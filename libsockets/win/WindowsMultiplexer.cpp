/*
 * WindowsMultiplexer.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include <iostream>

#include "SocketFactory.h"
#include "WindowsMultiplexer.h"
#include "WindowsSocket.h"

WindowsMultiplexer::WindowsMultiplexer(MultiplexerCallback callback) : MultiplexerImpl(callback) {
	/**
	 * Self-pipe trick to interrupt poll/select.
	 * Windows alternative to socketpair()
	 */
	auto server = socketFactory.CreateServerSocket();
	sockIn = socketFactory.CreateClientSocket();
	server->listenForConnections("127.0.0.1",""); // listen on a random free port
	sockIn->connectTo("127.0.0.1",server->getPort());
	sockIn->setNonBlockingIO(true);
	auto sockOut = server->acceptConnection();

	/* encapsulation breach!!! Due to socket FD data type,
	 * WindowsMultiplexer is coupled with WindowsSocket
	 */
	WindowsSocket *impl = static_cast<WindowsSocket *>(sockOut->getImpl().get());

	sockOutFD = impl->getFD();
	addClientSocket(std::move(sockOut));
}

WindowsMultiplexer::~WindowsMultiplexer() {
}

void WindowsMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);

	/* encapsulation breach!!! Due to socket FD data type,
	 * WindowsMultiplexer is coupled with WindowsSocket
	 */
	WindowsSocket *impl = static_cast<WindowsSocket *>(clientSocket->getImpl().get());
	auto fd = impl->getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	clients[fd]->setClientData(std::make_shared<ClientData>());
	interrupt();
}

void WindowsMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		std::shared_ptr<ClientData> clientData) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);

	/* encapsulation breach!!! Due to socket FD data type,
	 * WindowsMultiplexer is coupled with WindowsSocket
	 */
	WindowsSocket *impl = static_cast<WindowsSocket *>(clientSocket->getImpl().get());
	auto fd = impl->getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	clients[fd]->setClientData(clientData);
	interrupt();
}

std::unordered_map<std::shared_ptr<MultiplexedClientSocket>, std::pair<bool, bool>> WindowsMultiplexer::pollClients() {
	// <client, <read, write>>, if both read & write false then remove client
	std::unordered_map<std::shared_ptr<MultiplexedClientSocket>, std::pair<bool, bool>> readyClients;

	clientsMutex.lock();

	auto nfds = clients.size();
	WSAPOLLFD fdarray[nfds];

	auto clientIt = clients.begin();
	for (size_t i = 0; i < nfds; ++i, ++clientIt) {
		fdarray[i].fd = clientIt->first;
		fdarray[i].events = POLLIN;
		if (clientIt->second->getHasOutput()) {
			fdarray[i].events |= POLLOUT;
		}
	}

	clientsMutex.unlock();

	std::cout << "# " << nfds << std::endl;

	if (WSAPoll(fdarray,nfds,-1) > 0) {
		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto c:fdarray) {
			bool fdError = (c.revents & POLLERR) || (c.revents & POLLHUP) || (c.revents & POLLNVAL);
			bool readFlag = c.revents & POLLIN;
			bool writeFlag = c.revents & POLLOUT;
			auto client = clients[c.fd];

			if (fdError)
				readyClients[client] = std::make_pair(false, false); // mark for deletion
			else if (readFlag || writeFlag)
				readyClients[client] = std::make_pair(readFlag, writeFlag);
		}
	}

	return readyClients;
}

size_t WindowsMultiplexer::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size()-1; // self-pipe is always in clients
}

bool WindowsMultiplexer::selfPipe(std::shared_ptr<MultiplexedClientSocket> clientSocket) {
	WindowsSocket *impl = static_cast<WindowsSocket *>(clientSocket->getImpl().get());
	return impl->getFD() == sockOutFD;
}

void WindowsMultiplexer::removeClientSocket(std::shared_ptr<MultiplexedClientSocket> clientSocket) {
	clients.erase(static_cast<WindowsSocket *>(clientSocket->getImpl().get())->getFD());
}

