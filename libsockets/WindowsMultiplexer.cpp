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

WindowsMultiplexer::WindowsMultiplexer(MultiplexerCallback callback) : MultiplexerImpl(callback) {
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
	auto fd = impl->getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	interrupt();
}

void WindowsMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		MultiplexerCallback customCallback) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);

	/* encapsulation breach!!! Due to socket FD data type,
	 * WindowsMultiplexer is coupled with WindowsSocket
	 */
	WindowsSocket *impl = static_cast<WindowsSocket *>(clientSocket->getImpl().get());
	auto fd = impl->getFD();

	clients[fd] = makeMultiplexed(std::move(clientSocket));
	this->customCallback[fd] = customCallback;
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
			if (clientIt->second->getHasOutput()) {
				fdarray[i].events |= POLLOUT;
			}
		}
		clientsMutex.unlock();

		if (WSAPoll(fdarray,nfds,-1) > 0) {
			std::lock_guard<std::mutex> lock(clientsMutex);

			for (auto &c:fdarray) {
				bool readEvent = c.revents & POLLIN;
				bool writeEvent = c.revents & POLLOUT;
				if (readEvent || writeEvent) {
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
						MultiplexerCallback callback;

						auto cbIt = customCallback.find(c.fd);
						if (cbIt != customCallback.end())
							callback = cbIt->second;
						else
							callback = defaultCallback;

						if (!callback(client, readEvent, writeEvent)) {
							clients.erase(c.fd);
							if (cbIt != customCallback.end())
								customCallback.erase(c.fd);
						}
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

void WindowsMultiplexer::interrupt() {
	sendMultiplexerCommand(MultiplexerCommand::INTERRUPT);
}

size_t WindowsMultiplexer::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size()-1; // self-pipe is always in clients
}

void WindowsMultiplexer::sendMultiplexerCommand(int cmd) {
	std::lock_guard<std::mutex> lock(commandMutex);
	sockIn->sendData(static_cast<void *>(&cmd), sizeof(cmd));
}

