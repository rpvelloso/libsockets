/*
 * ClientSocket.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include "ClientSocket.h"

ClientSocket::ClientSocket(std::shared_ptr<SocketImpl> impl) : Socket(impl) {
	if (impl->getSocketState() == SocketStateType::Connected)
		sendBufferSize = state->getSendBufferSize();
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
