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

#include "DisconnectedState.h"

namespace socks {

DisconnectedState::DisconnectedState(SocketImpl &impl) : SocketState(impl) {
	setSocketState(SocketStateType::Disconnected);
	impl.setSocketState(socketState);
}

DisconnectedState::~DisconnectedState() {

}

std::pair<int, SocketAddress> DisconnectedState::receiveFrom(void* buf,
		size_t len) {
	return impl.receiveFrom(buf, len);
}

int DisconnectedState::sendTo(const SocketAddress& addr, const void* buf,
		size_t len) {
	return impl.sendTo(addr, buf, len);
}

int DisconnectedState::connectTo(const std::string &host, const std::string &port) {
	return impl.connectTo(host, port);
}

int DisconnectedState::bindSocket(const std::string &bindAddr, const std::string &port) {
	return impl.bindSocket(bindAddr, port);
}

int DisconnectedState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	return impl.listenForConnections(bindAddr, port);
}

}

