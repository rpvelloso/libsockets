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

#ifndef SRC_SOCKETS_SSL_SERVERSOCKET_H_
#define SRC_SOCKETS_SSL_SERVERSOCKET_H_

#include <string>

#include "Socket/Socket.h"
#include "Socket/SocketImpl.h"

namespace socks {

class ServerSocket : public Socket {
public:
	ServerSocket(ServerSocket &&) = default;
	ServerSocket(SocketImpl *impl);
	ServerSocket();
	virtual ~ServerSocket();
	int listenForConnections(const std::string &bindAddr, const std::string &port);
	ClientSocket acceptConnection();
	void disconnect();
};

}
#endif /* SRC_SOCKETS_SSL_SERVERSOCKET_H_ */
