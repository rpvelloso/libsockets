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

#include "Factory/SocketFactory.h"
#include "Multiplexer/MultiplexerImpl.h"
#include "Multiplexer/Poll.h"

namespace socks {

MultiplexerCallback defaultCallback = [](std::istream &inp, std::ostream &outp){}; // noop()

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

MultiplexerImpl::MultiplexerImpl(Poll *pollStrategy,
		MultiplexerCallback readCallbackFunc,
		MultiplexerCallback connectCallbackFunc,
		MultiplexerCallback disconnectCallbackFunc,
		MultiplexerCallback writeCallbackFunc) :
				pollStrategy(pollStrategy),
				readCallbackFunc(readCallbackFunc),
				connectCallbackFunc(connectCallbackFunc),
				disconnectCallbackFunc(disconnectCallbackFunc),
				writeCallbackFunc(writeCallbackFunc),
				clientCount(0) {

	auto socketPair = socketFactory.createSocketPair();
	sockIn = std::move(socketPair.first);
	sockOutFD = socketPair.second->getImpl().getFD();
	addClientSocket(std::move(socketPair.second));
}

MultiplexerImpl::~MultiplexerImpl() {
}

void MultiplexerImpl::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		MultiplexerCallback readCallbackFunc,
		MultiplexerCallback connectCallbackFunc,
		MultiplexerCallback disconnectCallbackFunc,
		MultiplexerCallback writeCallbackFunc) {

	std::lock_guard<std::mutex> lock(incomingClientsMutex);


	clientSocket->setNonBlockingIO(true);
	incomingClients.push_back(
		makeMultiplexed(
			std::move(clientSocket),
			readCallbackFunc,
			connectCallbackFunc,
			disconnectCallbackFunc,
			writeCallbackFunc)
	);
	interrupt();
}

void MultiplexerImpl::addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
	addClientSocket(
			std::move(clientSocket),
			this->readCallbackFunc,
			this->connectCallbackFunc,
			this->disconnectCallbackFunc,
			this->writeCallbackFunc);
}

void MultiplexerImpl::removeClientSocket(MultiplexedClientSocket &clientSocket) {
	clients.erase(clientSocket.getImpl().getFD());
}

size_t MultiplexerImpl::getClientCount() {
	return clientCount;
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
		auto readyClients = pollStrategy->pollClients(clients);

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
				auto ic = std::move(incomingClients[i]);
				clients[ic->getImpl().getFD()] = std::move(ic);
			}
			incomingClients.clear();
		}

		// update client count
		clientCount = clients.size() - 1; // subtract self-pipe from count
	}
}

std::unique_ptr<MultiplexedClientSocket> MultiplexerImpl::makeMultiplexed(
		std::unique_ptr<ClientSocket> clientSocket,
		MultiplexerCallback readCallbackFunc,
		MultiplexerCallback connectCallbackFunc,
		MultiplexerCallback disconnectCallbackFunc,
		MultiplexerCallback writeCallbackFunc
		) {
	auto mCli = std::make_unique<MultiplexedClientSocket>(
			std::move(clientSocket),
			readCallbackFunc,
			connectCallbackFunc,
			disconnectCallbackFunc,
			writeCallbackFunc);

	if (!selfPipe(*mCli))
		mCli->connectCallback();

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
				clientSocket.readCallback();
		}
	} catch (std::exception &e) {
		return false;
	}

	return true;
}

bool MultiplexerImpl::writeHandler(MultiplexedClientSocket &clientSocket) {
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
