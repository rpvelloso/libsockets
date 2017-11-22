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

#include "Socket/SocketState/ConnectedState.h"

namespace socks {

ConnectedState::ConnectedState(SocketImpl& impl) : SocketState(impl) {
	setSocketState(SocketStateType::Connected);
	impl.setSocketState(socketState);
};

ConnectedState::~ConnectedState() {

};

int ConnectedState::receiveData(void *buf, size_t len) {
	return impl.receiveData(buf, len);
};

int ConnectedState::sendData(const void *buf, size_t len) {
	return impl.sendData(buf, len);
};

std::pair<int, SocketAddress> ConnectedState::receiveFrom(void* buf,
		size_t len) {
	return impl.receiveFrom(buf, len);
};

int ConnectedState::sendTo(const SocketAddress& addr, const void* buf,
		size_t len) {
	return impl.sendTo(addr, buf, len);
};

void ConnectedState::disconnect() {
	impl.disconnect();
};

std::string ConnectedState::getPort() {
	return impl.getPort();
};

size_t ConnectedState::getSendBufferSize() {
	return impl.getSendBufferSize();
}

size_t ConnectedState::getReceiveBufferSize() {
	return impl.getReceiveBufferSize();
}

}
