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

#include "Socket/SocketState/ListeningState.h"

namespace socks {

ListeningState::ListeningState(SocketImpl &impl) : SocketState(impl) {
	setSocketState(SocketStateType::Listening);
	impl.setSocketState(socketState);
};

ListeningState::~ListeningState() {

};

int ListeningState::receiveData(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveData().");
};

int ListeningState::sendData(const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendData().");
};

std::pair<int, SocketAddress> ListeningState::receiveFrom(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveFrom().");
};

int ListeningState::sendTo(const SocketAddress &addr, const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendTo().");
};

int ListeningState::connectTo(const std::string &host, const std::string &port) {
	throw std::runtime_error("invalid operation connectTo().");
};

void ListeningState::disconnect() {
	impl.disconnect();
};

int ListeningState::bindSocket(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation bindSocket().");
};

int ListeningState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation listenForConnections().");
};

std::unique_ptr<SocketImpl> ListeningState::acceptConnection() {
	return impl.acceptConnection();
};

std::string ListeningState::getPort() {
	return impl.getPort();
}
;

size_t ListeningState::getSendBufferSize() {
	throw std::runtime_error("invalid operation getSendBufferSize().");
}

size_t ListeningState::getReceiveBufferSize() {
	throw std::runtime_error("invalid operation getReceiveBufferSize().");
}

}
