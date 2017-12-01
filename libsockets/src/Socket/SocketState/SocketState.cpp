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

#include <exception>

#include "Socket/SocketState/SocketState.h"

namespace socks {

SocketState::SocketState(SocketImpl &impl) : SocketImpl(), impl(impl) {};
SocketState::~SocketState() {};

int SocketState::receiveData(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveData().");
}

int SocketState::sendData(const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendData().");
}

std::pair<int, SocketAddress> SocketState::receiveFrom(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveFrom().");
}

int SocketState::sendTo(const SocketAddress &addr, const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendTo().");
}

int SocketState::connectTo(const std::string &host, const std::string &port) {
	throw std::runtime_error("invalid operation connectTo().");
}

void SocketState::disconnect() {
}

int SocketState::bindSocket(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation bindSocket().");
};

int SocketState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation listenForConnections().");
}

std::unique_ptr<SocketImpl> SocketState::acceptConnection() {
	throw std::runtime_error("invalid operation acceptConnection().");
}

std::string SocketState::getPort() {
	throw std::runtime_error("invalid operation getPort().");
}

size_t SocketState::getSendBufferSize() {
	throw std::runtime_error("invalid operation getSendBufferSize().");
}

size_t SocketState::getReceiveBufferSize() {
	throw std::runtime_error("invalid operation getReceiveBufferSize().");
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
