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

#ifndef SRC_SOCKET_SOCKETSTATE_DISCONNECTEDSTATE_H_
#define SRC_SOCKET_SOCKETSTATE_DISCONNECTEDSTATE_H_

#include "SocketState.h"

namespace socks {

class DisconnectedState : public SocketState {
public:
	DisconnectedState(SocketImpl &impl);
	virtual ~DisconnectedState();
	std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len) override;
	int sendTo(const SocketAddress &addr, const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	int bindSocket(const std::string &bindAddr, const std::string &port) override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
};

}

#endif /* SRC_SOCKET_SOCKETSTATE_DISCONNECTEDSTATE_H_ */
