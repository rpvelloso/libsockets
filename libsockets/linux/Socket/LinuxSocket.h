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

#ifndef LINUX_SOCKETS_LINUXSOCKET_H_
#define LINUX_SOCKETS_LINUXSOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "Socket/Socket.h"
#include "Socket/SocketImpl.h"
#include "Factory/FDFactory.h"
#include "defs.h"

namespace socks {

class LinuxMultiplexer;

class LinuxSocket: public SocketImpl {
	friend class LinuxSocketFactory;
public:
	LinuxSocket(FDFactory &fdFactory = TCPFDFactory);
	virtual ~LinuxSocket();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len);
	int sendTo(const SocketAddress &addr, const void *buf, size_t len);
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int bindSocket(const std::string &bindAddr, const std::string &port) override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<SocketImpl> acceptConnection() override;
	int setNonBlockingIO(bool status) override;
	int reuseAddress() override;
	std::string getPort() override;
	size_t getSendBufferSize() override;
	size_t getReceiveBufferSize() override;
	SocketAddress getLocalAddress() override;
	SocketAddress getRemoteAddress() override;
private:
	std::string port = "";

	LinuxSocket(SocketFDType); // ctor used by acceptConnections() & LinuxMultiplexer
};

using AddrResPtr = std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)>;

}
#endif /* LINUX_SOCKETS_LINUXSOCKET_H_ */
