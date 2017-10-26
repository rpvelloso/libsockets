/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "Socket/BufferedClientSocketInterface.h"
#include "Socket/BufferedClientSocket.h"
#include "Factory/SocketFactory.h"
#include "Multiplexer/MultiplexerImpl.h"
#include "Multiplexer/Poll.h"

namespace socks {

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

MultiplexerImpl::MultiplexerImpl(Poll *pollStrategy) :
		pollStrategy(pollStrategy),
		clientCount(0) {

	auto socketPair = socketFactory.createSocketPair();
	sockIn = std::move(socketPair.first);
	sockOutFD = socketPair.second->getImpl().getFD();
	std::unique_ptr<BufferedClientSocketInterface> sockOut(new BufferedClientSocket<size_t>(std::move(socketPair.second)));
	addClientSocket(std::move(sockOut));
}

MultiplexerImpl::~MultiplexerImpl() {
}

void MultiplexerImpl::addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket) {
	std::lock_guard<std::mutex> lock(incomingClientsMutex);


	clientSocket->setNonBlockingIO(true);
	clientSocket->connectCallback();
	incomingClients.push_back(std::move(clientSocket));
	interrupt();
}

void MultiplexerImpl::removeClientSocket(BufferedClientSocketInterface &clientSocket) {
	clients.erase(clientSocket.getImpl().getFD());
}

size_t MultiplexerImpl::getClientCount() {
	return clientCount;
}

bool MultiplexerImpl::selfPipe(BufferedClientSocketInterface &clientSocket) {
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

void MultiplexerImpl::multiplex(int timeout) {
	while (true) {
		auto readyClients = pollStrategy->pollClients(clients, timeout);

		for (auto rc:readyClients) {
			auto &clientSocket = std::get<0>(rc);
			auto readFlag = std::get<1>(rc);
			auto writeFlag = std::get<2>(rc);
			auto errorFlag = !(readFlag || writeFlag);
			auto sp = selfPipe(clientSocket);

			if (readFlag) {
				if (sp) {
					size_t cmdBufSize = 1024*sizeof(int);
					int cmdBuf[cmdBufSize], len, command;

					len = clientSocket.receiveData(static_cast<void *>(&cmdBuf),cmdBufSize);

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
					errorFlag = !readHandler(clientSocket);
				}
			}

			if (writeFlag && !errorFlag && !sp) {
				errorFlag = !writeHandler(clientSocket);
			}

			if (errorFlag) {
				if (!sp) {
					clientSocket.disconnectCallback();
					removeClientSocket(clientSocket);
				}
				else
					throw std::runtime_error("self-pipe error.");
			}
		}

		{ // process incoming clients
			std::lock_guard<std::mutex> lock(incomingClientsMutex);

			for (size_t i = 0; i < incomingClients.size(); ++i) {
				auto fd = incomingClients[i]->getImpl().getFD();
				clients[fd] = std::move(incomingClients[i]);
			}
			incomingClients.clear();
		}

		// update client count
		clientCount = clients.size() - 1; // subtract self-pipe from count
	}
}

bool MultiplexerImpl::readHandler(BufferedClientSocketInterface &clientSocket) {
	auto bufSize = clientSocket.getReceiveBufferSize();
	char buf[bufSize];
	int len;

	try {
		if ((len = clientSocket.receiveData(buf, bufSize)) > 0) {
			clientSocket.getInputBuffer().write(buf, len);

			if (!selfPipe(clientSocket))
				clientSocket.readCallback();
		}
	} catch (std::exception &e) {
		return false;
	}

	return true;
}

bool MultiplexerImpl::writeHandler(BufferedClientSocketInterface &clientSocket) {
	auto &outp = clientSocket.getOutputBuffer();

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

	clientSocket.writeCallback();

	return true;
}

}
