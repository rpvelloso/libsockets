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

				auto client = clients[c.fd];

				if (errorEvent)
					removeClient = true;
				else {
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
							auto &outputBuffer = client->getOutputBuffer();
							auto &inputBuffer = client->getInputBuffer();

							auto bufSize = client->getReceiveBufferSize();
							char buf[bufSize+1];
							int len;

							try {
								if ((len = client->receiveData(buf, bufSize)) <= 0) {
									client->setHangUp(true);
								} else {
									buf[len] = 0x00;
									inputBuffer.write(buf, len);

									readCallback(client);

									client->setHasOutput(outputBuffer.rdbuf()->in_avail() > 0);
								}
							} catch (std::exception &e) {
								removeClient = true;
							}
						}
					}

					if (writeEvent) {

						writeCallback(client);

						auto &outputBuffer = client->getOutputBuffer();

						while (outputBuffer.rdbuf()->in_avail() > 0) {
							int bufSize = client->getSendBufferSize();
							char buf[bufSize];

							auto savePos = outputBuffer.tellg();
							outputBuffer.readsome(buf, bufSize);
							try {
								if (client->sendData(buf, outputBuffer.gcount()) <= 0) {
									outputBuffer.clear();
									outputBuffer.seekg(savePos, outputBuffer.beg);
									break;
								}
							} catch (std::exception &e) {
								removeClient = true;
								break;
							}
						}

						if (outputBuffer.rdbuf()->in_avail() == 0) {
							outputBuffer.str(std::string());
							client->setHasOutput(false);
						} else
							client->setHasOutput(true);
					}

					if (client->getHangUp() && !client->getHasOutput())
						removeClient = true;
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

