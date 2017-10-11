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

#ifndef SRC_SOCKETS_SSL_SOCKETIMPL_H_
#define SRC_SOCKETS_SSL_SOCKETIMPL_H_

#include <cstddef>
#include <string>
#include <memory>

#include "defs.h"
#include "Socket/SocketAddress.h"

namespace socks {

class ClientSocket;

enum class SocketStateType {
	Disconnected = 0,
	Connected = 1,
	Listening = 2,
	Closed = 3
};

class SocketImpl {
public:
	SocketImpl() : socketState(SocketStateType::Disconnected) {};
	virtual ~SocketImpl() {};
	virtual int receiveData(void *buf, size_t len) = 0;
	virtual int sendData(const void *buf, size_t len) = 0;
	virtual std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len) = 0;
	virtual int sendTo(const SocketAddress &addr, const void *buf, size_t len) = 0;
	virtual int connectTo(const std::string &host, const std::string &port) = 0;
	virtual void disconnect() = 0;
	virtual int bindSocket(const std::string &bindAddr, const std::string &port) = 0;
	virtual int listenForConnections(const std::string &bindAddr, const std::string &port) = 0;
	virtual std::unique_ptr<SocketImpl> acceptConnection() = 0;
	virtual int setNonBlockingIO(bool status) = 0;
	virtual int reuseAddress() = 0;
	virtual std::string getPort() = 0;
	virtual size_t getSendBufferSize() = 0;
	virtual size_t getReceiveBufferSize() = 0;
	virtual SocketStateType getSocketState() {
		return socketState;
	};
	virtual void setSocketState(SocketStateType socketState) {
		this->socketState = socketState;
	};
	virtual SocketFDType getFD() {
		return fd;
	}
protected:
	SocketImpl(SocketFDType fd) : fd(fd), socketState(SocketStateType::Connected) {};
	SocketFDType fd = InvalidSocketFD;
	SocketStateType socketState;
};

}
#endif /* SRC_SOCKETS_SSL_SOCKETIMPL_H_ */
