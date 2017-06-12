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

LinuxMultiplexer::LinuxMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) :
		MultiplexerImpl(readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback) {
	int selfPipe[2];

	socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, selfPipe);
	sockIn = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[0]));
	auto sockOut = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[1]));;
	sockOutFD = selfPipe[1];
	addClientSocket(std::move(sockOut), std::make_unique<ClientData>());
}

LinuxMultiplexer::~LinuxMultiplexer() {
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
			bool fdError = false;
			bool readFlag = c.revents & POLLIN;
			bool writeFlag = c.revents & POLLOUT;
			if (!(readFlag || writeFlag))
				fdError = (c.revents & POLLERR) || (c.revents & POLLHUP) || (c.revents & POLLNVAL);
			auto &client = *clients[c.fd];

			if (fdError)
				readyClients.push_back(std::forward_as_tuple(client, false, false));
			else if (readFlag || writeFlag)
				readyClients.push_back(std::forward_as_tuple(client, readFlag, writeFlag));
		}
	}

	return readyClients;
}
