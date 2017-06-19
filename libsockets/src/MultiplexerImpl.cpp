/*
 * MultiplexerImpl.cpp
 *
 *  Created on: 1 de jun de 2017
 *      Author: rvelloso
 */

#include <iostream>

#include "MultiplexerImpl.h"
#include "SocketFactory.h"
#include "Poll.h"

namespace socks {

MultiplexerCallback defaultCallback = [](std::istream &inp, std::ostream &outp, ClientData&){}; // noop()

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

MultiplexerImpl::MultiplexerImpl(Poll *pollStrategy,
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) :
				pollStrategy(pollStrategy),
				readCallback(readCallback),
				connectCallback(connectCallback),
				disconnectCallback(disconnectCallback),
				writeCallback(writeCallback) {

	auto socketPair = socketFactory.createSocketPair();
	sockIn = std::move(socketPair.first);
	sockOutFD = socketPair.second->getImpl().getFD();
	addClientSocket(std::move(socketPair.second), std::make_unique<ClientData>());
}

MultiplexerImpl::~MultiplexerImpl() {
}

void MultiplexerImpl::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		std::unique_ptr<ClientData> clientData) {
	std::lock_guard<std::mutex> lock(clientsMutex);

	clientSocket->setNonBlockingIO(true);
	auto fd = clientSocket->getImpl().getFD();
	clients[fd] = makeMultiplexed(std::move(clientSocket), std::move(clientData));
	interrupt();
}

void MultiplexerImpl::removeClientSocket(MultiplexedClientSocket &clientSocket) {
	clients.erase(clientSocket.getImpl().getFD());
}

size_t MultiplexerImpl::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size()-1; // self-pipe is always in clients
}

bool MultiplexerImpl::selfPipe(MultiplexedClientSocket &clientSocket) {
	return clientSocket.getImpl().getFD() == sockOutFD;
}

void MultiplexerImpl::interrupt() {
	sendMultiplexerCommand(MultiplexerCommand::INTERRUPT);
}

void MultiplexerImpl::cancel() {
	sendMultiplexerCommand(MultiplexerCommand::CANCEL);
}

void MultiplexerImpl::sendMultiplexerCommand(int cmd) {
	std::lock_guard<std::mutex> lock(commandMutex);
	sockIn->sendData(static_cast<void *>(&cmd), sizeof(cmd));
}

void MultiplexerImpl::multiplex() {
	while (true) {
		auto readyClients = pollStrategy->pollClients(clients, clientsMutex);

		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto rc:readyClients) {
			auto &client = std::get<0>(rc);
			auto readFlag = std::get<1>(rc);
			auto writeFlag = std::get<2>(rc);
			auto errorFlag = !(readFlag || writeFlag);
			auto sp = selfPipe(client);

			if (readFlag) {
				if (sp) {
					size_t cmdBufSize = 1024*sizeof(int);
					int cmdBuf[cmdBufSize], len, command;

					len = client.receiveData(static_cast<void *>(&cmdBuf),cmdBufSize);

					for (int i = 0; i < (int)(len/sizeof(int)); ++i) {
						command = cmdBuf[i];
						if (cmdBuf[i] == MultiplexerCommand::CANCEL)
							break;
					}

					switch (command) {
					case MultiplexerCommand::INTERRUPT:
						break;
					case MultiplexerCommand::CANCEL:
						return;
					}
				} else {
					errorFlag = !readHandler(client);
				}
			}

			if (writeFlag && !errorFlag && !sp) {
				errorFlag = !writeHandler(client);
			}

			if (errorFlag) {
				if (!sp) {
					disconnectCallback(
							client.getInputBuffer(),
							client.getInputBuffer(),
							client.getClientData());
					removeClientSocket(client);
				}
				else
					throw std::runtime_error("self-pipe error.");
			}
		}
	}
}

std::unique_ptr<MultiplexedClientSocket> MultiplexerImpl::makeMultiplexed(
		std::unique_ptr<ClientSocket> clientSocket,
		std::unique_ptr<ClientData> clientData) {
	auto mCli = std::make_unique<MultiplexedClientSocket>(
			std::move(clientSocket),
			std::move(clientData),
			std::bind(&MultiplexerImpl::interrupt, this));

	if (!selfPipe(*mCli))
		connectCallback(
				mCli->getInputBuffer(),
				mCli->getOutputBuffer(),
				mCli->getClientData());

	return std::move(mCli);
};

bool MultiplexerImpl::readHandler(MultiplexedClientSocket &clientSocket) {
	auto bufSize = clientSocket.getReceiveBufferSize();
	char buf[bufSize];
	int len;

	try {
		if ((len = clientSocket.receiveData(buf, bufSize)) > 0) {
			clientSocket.getInputBuffer().write(buf, len);

			if (!selfPipe(clientSocket))
				readCallback(
						clientSocket.getInputBuffer(),
						clientSocket.getOutputBuffer(),
						clientSocket.getClientData());
		}
	} catch (std::exception &e) {
		return false;
	}

	return true;
}

bool MultiplexerImpl::writeHandler(MultiplexedClientSocket &clientSocket) {
	auto &outp = clientSocket.getOutputBuffer();
	auto &inp = clientSocket.getInputBuffer();

	while (outp.rdbuf()->in_avail() > 0) {
		int bufSize = clientSocket.getSendBufferSize();
		char buf[bufSize];

		auto savePos = outp.tellg();
		outp.readsome(buf, bufSize);
		try {
			if (clientSocket.sendData(buf, outp.gcount()) <= 0) {
				outp.clear();
				outp.seekg(savePos, outp.beg);
				break;
			}
		} catch (std::exception &e) {
			return false;
		}
	}

	if (!clientSocket.getHasOutput()) {
		outp.clear();
		outp.str(std::string());
	}

	writeCallback(inp, outp, clientSocket.getClientData());

	return true;
}

}
