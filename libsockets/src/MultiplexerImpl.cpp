/*
 * MultiplexerImpl.cpp
 *
 *  Created on: 1 de jun de 2017
 *      Author: rvelloso
 */

#include "MultiplexerImpl.h"

enum MultiplexerCommand : int {
	CANCEL = 0x00,
	INTERRUPT = 0x01
};

MultiplexerImpl::MultiplexerImpl(MultiplexerCallback callback) : callback(callback) {
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
					int cmd;

					client.receiveData(static_cast<void *>(&cmd),sizeof(cmd));
					switch (cmd) {
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
					removeClientSocket(client);
				}
				else
					throw std::runtime_error("self-pipe error..");
			}
		}
	}
}

std::unique_ptr<MultiplexedClientSocket> MultiplexerImpl::makeMultiplexed(std::unique_ptr<ClientSocket> clientSocket) {
	return std::make_unique<MultiplexedClientSocket>(std::move(clientSocket), std::bind(&MultiplexerImpl::interrupt, this));
};

bool MultiplexerImpl::readHandler(MultiplexedClientSocket &client) {
	auto &outputBuffer = client.getOutputBuffer();
	auto &inputBuffer = client.getInputBuffer();

	auto bufSize = client.getReceiveBufferSize();
	char buf[bufSize+1];
	int len;

	try {
		if ((len = client.receiveData(buf, bufSize)) <= 0) {
			client.setHangUp(true);
		} else {
			buf[len] = 0x00;
			inputBuffer.write(buf, len);

			callback(inputBuffer, outputBuffer, client.getClientData());

			client.setHasOutput(outputBuffer.rdbuf()->in_avail() > 0);
		}
	} catch (std::exception &e) {
		return false;
	}

	return true;
}

bool MultiplexerImpl::writeHandler(MultiplexedClientSocket &client) {
	auto &outputBuffer = client.getOutputBuffer();

	while (outputBuffer.rdbuf()->in_avail() > 0) {
		int bufSize = client.getSendBufferSize();
		char buf[bufSize];

		auto savePos = outputBuffer.tellg();
		outputBuffer.readsome(buf, bufSize);
		try {
			if (client.sendData(buf, outputBuffer.gcount()) <= 0) {
				outputBuffer.clear();
				outputBuffer.seekg(savePos, outputBuffer.beg);
				break;
			}
		} catch (std::exception &e) {
			return false;
		}
	}

	if (outputBuffer.rdbuf()->in_avail() == 0) {
		outputBuffer.str(std::string());
		client.setHasOutput(false);
	} else
		client.setHasOutput(true);

	return true;
}
