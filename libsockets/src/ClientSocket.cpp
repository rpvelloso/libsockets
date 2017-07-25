/*
 * ClientSocket.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include "ClientSocket.h"
#include "SocketFactory.h"

namespace socks {

ClientSocket& ClientSocket::operator =(ClientSocket&& rhs) {
	Socket::operator =(std::move(rhs));
	this->receiveBufferSize = rhs.receiveBufferSize;
	this->sendBufferSize = rhs.sendBufferSize;
	return *this;
}

ClientSocket::ClientSocket(SocketImpl *impl) : Socket(impl) {
	if (impl->getSocketState() == SocketStateType::Connected) {
		sendBufferSize = state->getSendBufferSize();
		receiveBufferSize = state->getReceiveBufferSize();
	}
}

ClientSocket::ClientSocket() : Socket(socketFactory.createSocketImpl()) {

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
		state.reset(new ConnectedState(getImpl()));
		sendBufferSize = state->getSendBufferSize();
		receiveBufferSize = state->getReceiveBufferSize();
	}

	return ret;
}

void ClientSocket::disconnect() {
	state->disconnect();
	state.reset(new ClosedState(getImpl()));
}

size_t ClientSocket::getSendBufferSize() const {
	return sendBufferSize;
}

size_t ClientSocket::getReceiveBufferSize() const {
	return receiveBufferSize;
}

}
