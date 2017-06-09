/*
 * MultiplexerImpl.cpp
 *
 *  Created on: 1 de jun de 2017
 *      Author: rvelloso
 */

#include <iostream>

#include "MultiplexerImpl.h"

MultiplexerCallback defaultCallback = [](std::istream &inp, std::ostream &outp, ClientData&){}; // noop()

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

MultiplexerImpl::MultiplexerImpl(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) :
				readCallback(readCallback),
				connectCallback(connectCallback),
				disconnectCallback(disconnectCallback),
				writeCallback(writeCallback) {
}

MultiplexerImpl::~MultiplexerImpl() {
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
		auto readyClients = pollClients();

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

			if (client.getHangUp() && !client.getHasOutput())
				errorFlag = true;

			if (errorFlag) {
				if (!sp) {
					auto &inp = client.getInputBuffer();
					auto &outp = client.getInputBuffer();
					auto &cliData = client.getClientData();
					disconnectCallback(inp, outp, cliData);
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

	if (!selfPipe(*mCli)) {
		auto &inp = mCli->getInputBuffer();
		auto &outp = mCli->getOutputBuffer();
		auto &cliData = mCli->getClientData();
		connectCallback(inp, outp, cliData);
	}

	return std::move(mCli);
};

bool MultiplexerImpl::readHandler(MultiplexedClientSocket &clientSocket) {
	auto &outp = clientSocket.getOutputBuffer();
	auto &inp = clientSocket.getInputBuffer();

	auto bufSize = clientSocket.getReceiveBufferSize();
	char buf[bufSize+1];
	int len;

	try {
		if ((len = clientSocket.receiveData(buf, bufSize)) <= 0) {
			clientSocket.setHangUp(true);
		} else {
			buf[len] = 0x00;
			inp.write(buf, len);

			if (!selfPipe(clientSocket))
				readCallback(inp, outp, clientSocket.getClientData());
		}
	} catch (std::exception &e) {
		return false;
	}

	return true;
}

bool MultiplexerImpl::writeHandler(MultiplexedClientSocket &clientSocket) {
	auto &outp = clientSocket.getOutputBuffer();
	auto &inp = clientSocket.getInputBuffer();

	writeCallback(inp, outp, clientSocket.getClientData());

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

	return true;
}
