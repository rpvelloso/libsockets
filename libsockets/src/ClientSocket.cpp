/*
 * ClientSocket.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include "ClientSocket.h"
#include <iostream>

ClientSocket::ClientSocket(std::shared_ptr<SocketImpl> impl) : Socket(impl) {
	if (impl->getSocketState() == SocketStateType::Connected) {
		sendBufferSize = state->getSendBufferSize();
		receiveBufferSize = state->getReceiveBufferSize();
		std::cout << "LST: " << sendBufferSize << " " << receiveBufferSize << std::endl;
	}
}

ClientSocket::~ClientSocket() {
}

int ClientSocket::receiveData(void* buf, size_t len) {
	return state->receiveData(buf, len);
}

int ClientSocket::sendData(const void* buf, size_t len) {
	return state->sendData(buf, len);
}

int ClientSocket::connectTo(const std::string &host, const std::string &port) {
	int ret;

	if ((ret = state->connectTo(host, port)) == 0) {
		state.reset(new ConnectedState(impl));
		sendBufferSize = state->getSendBufferSize();
		receiveBufferSize = state->getReceiveBufferSize();
		std::cout << "CNT: " << sendBufferSize << " " << receiveBufferSize << std::endl;
	}

	return ret;
}

void ClientSocket::disconnect() {
	state->disconnect();
	state.reset(new ClosedState(impl));
}

size_t ClientSocket::getSendBufferSize() const {
	return sendBufferSize;
}

size_t ClientSocket::getReceiveBufferSize() const {
	return receiveBufferSize;
}
