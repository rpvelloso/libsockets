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

#include "Socket/SocketState/ClosedState.h"

namespace socks {

ClosedState::ClosedState(SocketImpl &impl) : SocketState(impl) {
	setSocketState(SocketStateType::Closed);
	impl.setSocketState(socketState);
};

ClosedState::~ClosedState() {

};

int ClosedState::receiveData(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveData().");
};

int ClosedState::sendData(const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendData().");
};

std::pair<int, SocketAddress> ClosedState::receiveFrom(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveFrom().");
};

int ClosedState::sendTo(const SocketAddress &addr, const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendTo().");
};

int ClosedState::connectTo(const std::string &host, const std::string &port) {
	throw std::runtime_error("invalid operation connectTo().");
};

void ClosedState::disconnect() {
};

int ClosedState::bindSocket(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation bindSocket().");
};

int ClosedState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation listenForConnections().");
};

std::unique_ptr<SocketImpl> ClosedState::acceptConnection() {
	throw std::runtime_error("invalid operation acceptConnection().");
};

std::string ClosedState::getPort() {
	throw std::runtime_error("invalid operation getPort().");
}
;

size_t ClosedState::getSendBufferSize() {
	throw std::runtime_error("invalid operation getSendBufferSize().");
}

size_t ClosedState::getReceiveBufferSize() {
	throw std::runtime_error("invalid operation getReceiveBufferSize().");
}
}
