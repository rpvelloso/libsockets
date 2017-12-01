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

#include "Socket/ClientSocket.h"
#include "Factory/SocketFactory.h"
#include "Socket/SocketState/ConnectedState.h"
#include "Socket/SocketState/ClosedState.h"

namespace socks {

namespace factory {
	ClientSocket makeClientSocket() {
		return ClientSocket(socketFactory.createSocketImpl());
	}
}

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
