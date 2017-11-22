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

#ifndef SRC_SOCKET_SOCKETSTATE_SOCKETSTATE_H_
#define SRC_SOCKET_SOCKETSTATE_SOCKETSTATE_H_

#include <exception>

#include "Socket/SocketImpl.h"

namespace socks {

class SocketState : public SocketImpl {
public:
	SocketState(SocketImpl &impl) : SocketImpl(), impl(impl) {};
	virtual ~SocketState() {};

	int receiveData(void *buf, size_t len) {
		throw std::runtime_error("invalid operation receiveData().");
	}

	int sendData(const void *buf, size_t len) {
		throw std::runtime_error("invalid operation sendData().");
	}

	std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len) {
		throw std::runtime_error("invalid operation receiveFrom().");
	}

	int sendTo(const SocketAddress &addr, const void *buf, size_t len) {
		throw std::runtime_error("invalid operation sendTo().");
	}

	int connectTo(const std::string &host, const std::string &port) {
		throw std::runtime_error("invalid operation connectTo().");
	}

	void disconnect() {
	}

	int bindSocket(const std::string &bindAddr, const std::string &port) {
		throw std::runtime_error("invalid operation bindSocket().");
	};

	int listenForConnections(const std::string &bindAddr, const std::string &port) {
		throw std::runtime_error("invalid operation listenForConnections().");
	}

	std::unique_ptr<SocketImpl> acceptConnection() {
		throw std::runtime_error("invalid operation acceptConnection().");
	}

	std::string getPort() {
		throw std::runtime_error("invalid operation getPort().");
	}

	size_t getSendBufferSize() {
		throw std::runtime_error("invalid operation getSendBufferSize().");
	}

	size_t getReceiveBufferSize() {
		throw std::runtime_error("invalid operation getReceiveBufferSize().");
	}

	int setNonBlockingIO(bool status) override {
		return impl.setNonBlockingIO(status);
	}

	int reuseAddress() override {
		return impl.reuseAddress();
	}

	SocketAddress getLocalAddress() {
		return impl.getLocalAddress();
	}

	SocketAddress getRemoteAddress() {
		return impl.getRemoteAddress();
	}

protected:
	SocketImpl &impl;
};

}
#endif /* SRC_SOCKET_SOCKETSTATE_SOCKETSTATE_H_ */
