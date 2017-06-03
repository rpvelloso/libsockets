/*
 * LinuxMultiplexer.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include <iostream>

#include <poll.h>
#include "SocketFactory.h"
#include "LinuxMultiplexer.h"
#include "LinuxSocket.h"

LinuxMultiplexer::LinuxMultiplexer(MultiplexerCallback callback) : MultiplexerImpl(callback) {
	int selfPipe[2];

	/* encapsulation breach!!! Due to socket FD data type,
	 * LinuxMultiplexer is coupled with LinuxSocket
	 */
	socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, selfPipe);
	sockIn = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[0]));
	auto sockOut = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[1]));;
	sockOutFD = selfPipe[1];
	addClientSocket(std::move(sockOut));
}

LinuxMultiplexer::~LinuxMultiplexer() {
}

void LinuxMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);

	/* encapsulation breach!!! Due to socket FD data type,
	 * LinuxMultiplexer is coupled with LinuxSocket
	 */
	auto impl = static_cast<LinuxSocket &>(clientSocket->getImpl());
	auto fd = impl.getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	clients[fd]->setClientData(std::make_unique<ClientData>());
	interrupt();
}

void LinuxMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		std::unique_ptr<ClientData> clientData) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);

	/* encapsulation breach!!! Due to socket FD data type,
	 * LinuxMultiplexer is coupled with LinuxSocket
	 */
	auto impl = static_cast<LinuxSocket &>(clientSocket->getImpl());
	auto fd = impl.getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	clients[fd]->setClientData(std::move(clientData));
	interrupt();
}

size_t LinuxMultiplexer::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size()-1; // self-pipe is always in clients
}

void LinuxMultiplexer::removeClientSocket(MultiplexedClientSocket &clientSocket) {
	clients.erase(static_cast<LinuxSocket &>(clientSocket.getImpl()).getFD());
}

bool LinuxMultiplexer::selfPipe(MultiplexedClientSocket &clientSocket) {
	auto impl = static_cast<LinuxSocket &>(clientSocket.getImpl());
	return impl.getFD() == sockOutFD;
}

std::vector<pollTuple> LinuxMultiplexer::pollClients() {
	// <client, <read, write>>, if both read & write false then remove client
	std::vector<pollTuple> readyClients;

	clientsMutex.lock();

	auto nfds = clients.size();
	struct pollfd fdarray[nfds];

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

	if (poll(fdarray,nfds,-1) > 0) {
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
