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

#include "Socket/SocketImpl.h"

namespace socks {

class SocketState : public SocketImpl {
public:
	SocketState(SocketImpl &impl);
	virtual ~SocketState();
	int receiveData(void *buf, size_t len);
	int sendData(const void *buf, size_t len);
	std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len);
	int sendTo(const SocketAddress &addr, const void *buf, size_t len);
	int connectTo(const std::string &host, const std::string &port);
	void disconnect();
	int bindSocket(const std::string &bindAddr, const std::string &port);
	int listenForConnections(const std::string &bindAddr, const std::string &port);
	std::unique_ptr<SocketImpl> acceptConnection();
	std::string getPort();
	size_t getSendBufferSize();
	size_t getReceiveBufferSize();
	int setNonBlockingIO(bool status) override;
	int reuseAddress() override;
	SocketAddress getLocalAddress();
	SocketAddress getRemoteAddress();
protected:
	SocketImpl &impl;
};

}
#endif /* SRC_SOCKET_SOCKETSTATE_SOCKETSTATE_H_ */
