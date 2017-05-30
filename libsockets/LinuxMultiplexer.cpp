/*
 * LinuxMultiplexer.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include <poll.h>
#include "SocketFactory.h"
#include "LinuxMultiplexer.h"
#include "LinuxSocket.h"

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

LinuxMultiplexer::LinuxMultiplexer(MultiplexerCallback readCallback, MultiplexerCallback writeCallback) : MultiplexerImpl(readCallback, writeCallback) {
	int selfPipe[2];

	/* encapsulation breach!!! Due to socket FD data type,
	 * LinuxMultiplexer is coupled with LinuxSocket
	 */
	socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, selfPipe);
	std::shared_ptr<SocketImpl> implIn(new LinuxSocket(selfPipe[0]));
	std::shared_ptr<SocketImpl> implOut(new LinuxSocket(selfPipe[1]));
	sockIn = std::make_unique<ClientSocket>(implIn);
	auto sockOut = std::make_unique<ClientSocket>(implOut);;
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
	LinuxSocket *impl = static_cast<LinuxSocket *>(clientSocket->getImpl().get());
	auto fd = impl->getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	clients[fd]->setClientData(std::make_shared<ClientData>());
	interrupt();
}

void LinuxMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		std::shared_ptr<ClientData> clientData) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);

	/* encapsulation breach!!! Due to socket FD data type,
	 * LinuxMultiplexer is coupled with LinuxSocket
	 */
	LinuxSocket *impl = static_cast<LinuxSocket *>(clientSocket->getImpl().get());
	auto fd = impl->getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	clients[fd]->setClientData(clientData);
	interrupt();
}

void LinuxMultiplexer::multiplex() {
	while (true) {
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

		if (poll(fdarray,nfds,-1) > 0) {
			std::lock_guard<std::mutex> lock(clientsMutex);

			for (auto &c:fdarray) {
				bool readEvent = c.revents & POLLIN;
				bool writeEvent = c.revents & POLLOUT;
				bool errorEvent = (c.revents & POLLERR) || (c.revents & POLLHUP);
				bool removeClient = false;

				if (errorEvent)
					removeClient = true;
				else {
					auto client = clients[c.fd];

					if (readEvent) {
						if (c.fd == sockOutFD) {
							int cmd;

							client->receiveData(static_cast<void *>(&cmd),sizeof(cmd));
							switch (cmd) {
							case MultiplexerCommand::INTERRUPT:
								break;
							case MultiplexerCommand::CANCEL:
								return;
							}
						} else {
							removeClient = !readCallback(client);
						}
					}

					if (writeEvent) {
						if (!removeClient)
							removeClient = !writeCallback(client);
					}
				}

				if (removeClient)
					clients.erase(c.fd);
			}
		} else
			break;
	}
}

void LinuxMultiplexer::cancel() {
	sendMultiplexerCommand(MultiplexerCommand::CANCEL);
}

void LinuxMultiplexer::interrupt() {
	sendMultiplexerCommand(MultiplexerCommand::INTERRUPT);
}

size_t LinuxMultiplexer::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size()-1; // self-pipe is always in clients
}

void LinuxMultiplexer::sendMultiplexerCommand(int cmd) {
	std::lock_guard<std::mutex> lock(commandMutex);
	sockIn->sendData(static_cast<void *>(&cmd), sizeof(cmd));
}

