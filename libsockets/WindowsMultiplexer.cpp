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

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

WindowsMultiplexer::WindowsMultiplexer(std::function<bool(std::shared_ptr<ClientSocket>)> callback) : MultiplexerImpl(callback) {
	auto server = socketFactory->CreateServerSocket();
	sockIn = socketFactory->CreateClientSocket();
	server->listenForConnections("127.0.0.1","60000");
	sockIn->connectTo("127.0.0.1","60000");
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
	clients[impl->getFD()] = std::move(clientSocket);
	interrupt();
}

void WindowsMultiplexer::multiplex() {
	while (true) {
		clientsMutex.lock();
		auto nfds = clients.size();
		WSAPOLLFD fdarray[nfds];

		size_t i = 0;
		for (auto &c:clients) {
			fdarray[i].fd = c.first;
			fdarray[i].events = POLLIN;
			std::cout << c.first << std::endl;
		}
		std::cout <<"***"<< std::endl;
		clientsMutex.unlock();

		std::cout << "polling..." << std::endl;

		if (WSAPoll(fdarray,nfds,-1) > 0) {
			std::lock_guard<std::mutex> lock(clientsMutex);

			for (auto &c:fdarray) {
				if (c.revents & POLLIN) {
					auto client = clients[c.fd];
					std::cout << c.fd << " " << sockOutFD << " " << nfds << std::endl;
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
						if (!callback(client))
							clients.erase(c.fd);
					}
				}
			}
		} else
			break;
	}
}

void WindowsMultiplexer::cancel() {
	sendMultiplexerCommand(MultiplexerCommand::CANCEL);
}

size_t WindowsMultiplexer::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size();
}

void WindowsMultiplexer::sendMultiplexerCommand(int cmd) {
	sockIn->sendData(static_cast<void *>(&cmd), sizeof(cmd));
}

void WindowsMultiplexer::interrupt() {
	sendMultiplexerCommand(MultiplexerCommand::INTERRUPT);
}

void WindowsMultiplexer::removeClientSocket(SOCKET fd) {
	std::lock_guard<std::mutex> lock(clientsMutex);
	clients.erase(fd);
}
