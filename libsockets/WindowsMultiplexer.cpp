/*
 * WindowsMultiplexer.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "SocketFactory.h"
#include "WindowsMultiplexer.h"
#include "WindowsSocket.h"

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

WindowsMultiplexer::WindowsMultiplexer(std::function<bool(std::shared_ptr<ClientSocket>)> callback) : MultiplexerImpl(callback) {
	/**
	 * Self-pipe trick to interrupt poll/select.
	 * Windows alternative to socketpair()
	 */
	auto server = socketFactory->CreateServerSocket();
	sockIn = socketFactory->CreateClientSocket();
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
	clients[impl->getFD()] = std::move(clientSocket);
	interrupt();
}

void WindowsMultiplexer::multiplex() {
	while (true) {
		clientsMutex.lock();
		auto nfds = clients.size();
		WSAPOLLFD fdarray[nfds];

		auto clientIt = clients.begin();
		for (size_t i = 0; i < nfds; ++i, ++clientIt) {
			fdarray[i].fd = clientIt->first;
			fdarray[i].events = POLLIN;
		}
		clientsMutex.unlock();

		if (WSAPoll(fdarray,nfds,-1) > 0) {
			std::lock_guard<std::mutex> lock(clientsMutex);

			for (auto &c:fdarray) {
				if (c.revents & POLLIN) {
					auto client = clients[c.fd];
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
				} else if ((c.revents & POLLERR) || (c.revents & POLLHUP))
					clients.erase(c.fd);
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
	std::lock_guard<std::mutex> lock(commandMutex);
	sockIn->sendData(static_cast<void *>(&cmd), sizeof(cmd));
}

void WindowsMultiplexer::interrupt() {
	sendMultiplexerCommand(MultiplexerCommand::INTERRUPT);
}
