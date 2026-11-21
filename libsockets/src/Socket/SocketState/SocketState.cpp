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

#include "SocketState.h"

namespace socks {

SocketState::SocketState(SocketImpl &impl) : impl(impl)
{}

SocketState::~SocketState()
{}

// a state that does not support an operation fails it instead of throwing:
// -1 for the int operations, an empty result for the others
int SocketState::receiveData(void *buf, size_t len) {
	return -1;
}

SendDataResult SocketState::sendData(const void *buf, size_t len) {
	return {-1, 0, false};
}

std::pair<int, SocketAddress> SocketState::receiveFrom(void *buf, size_t len) {
	return {-1, SocketAddress(nullptr)};
}

int SocketState::sendTo(const SocketAddress &addr, const void *buf, size_t len) {
	return -1;
}

int SocketState::connectTo(const std::string &host, const std::string &port) {
	return -1;
}

void SocketState::disconnect() {
}

int SocketState::bindSocket(const std::string &bindAddr, const std::string &port) {
	return -1;
};

int SocketState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	return -1;
}

AcceptResult SocketState::acceptConnection() {
	return {};
}

std::string SocketState::getPort() {
	return "";
}

size_t SocketState::getSendBufferSize() {
	return 0;
}

size_t SocketState::getReceiveBufferSize() {
	return 0;
}

int SocketState::setNonBlockingIO(bool status) {
	return impl.setNonBlockingIO(status);
}

int SocketState::reuseAddress() {
	return impl.reuseAddress();
}

SocketAddress SocketState::getLocalAddress() {
	return impl.getLocalAddress();
}

SocketAddress SocketState::getRemoteAddress() {
	return impl.getRemoteAddress();
}

}
