/*
 * WindowsMultiplexer.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include <iostream>

#include "MultiplexerImpl.h"
#include "SocketFactory.h"
#include "WindowsMultiplexer.h"
#include "WindowsSocket.h"

WindowsMultiplexer::WindowsMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) :
		MultiplexerImpl(readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback) {
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
	auto impl = static_cast<WindowsSocket &>(sockOut->getImpl());

	sockOutFD = impl.getFD();
	addClientSocket(std::move(sockOut), std::make_unique<ClientData>());
}

WindowsMultiplexer::~WindowsMultiplexer() {
}

void WindowsMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		std::unique_ptr<ClientData> clientData) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);

	/* encapsulation breach!!! Due to socket FD data type,
	 * WindowsMultiplexer is coupled with WindowsSocket
	 */
	auto impl = static_cast<WindowsSocket &>(clientSocket->getImpl());
	auto fd = impl.getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket), std::move(clientData));
	interrupt();
}

std::vector<pollTuple> WindowsMultiplexer::pollClients() {
	// <client, read, write>, if both read & write false then remove client
	std::vector<pollTuple> readyClients;

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

	//std::cout << "# " << nfds << std::endl;

	if (WSAPoll(fdarray,nfds,-1) > 0) {
		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto c:fdarray) {
			bool fdError = (c.revents & POLLERR) || (c.revents & POLLHUP) || (c.revents & POLLNVAL);
			bool readFlag = c.revents & POLLIN;
			bool writeFlag = c.revents & POLLOUT;
			auto &client = *clients[c.fd];

			if (fdError)
				readyClients.push_back(std::forward_as_tuple(client, false, false));
			else if (readFlag || writeFlag)
				readyClients.push_back(std::forward_as_tuple(client, readFlag, writeFlag));
		}
	}

	return readyClients;
}

size_t WindowsMultiplexer::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size()-1; // self-pipe is always in clients
}

bool WindowsMultiplexer::selfPipe(MultiplexedClientSocket &clientSocket) {
	auto impl = static_cast<WindowsSocket &>(clientSocket.getImpl());
	return impl.getFD() == sockOutFD;
}

void WindowsMultiplexer::removeClientSocket(MultiplexedClientSocket &clientSocket) {
	clients.erase(static_cast<WindowsSocket &>(clientSocket.getImpl()).getFD());
}

