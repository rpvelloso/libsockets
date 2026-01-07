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

#ifndef SRC_SOCKETS_CLIENTSOCKET_H_
#define SRC_SOCKETS_CLIENTSOCKET_H_

#include <memory>
#include <functional>

#include "Socket.h"

namespace socks {

class SocketImpl;

class ClientSocket : public Socket {
public:
	ClientSocket(ClientSocket &&) = default;
	ClientSocket &operator=(ClientSocket &&);

	ClientSocket(SocketImpl *impl);
	ClientSocket();
	virtual ~ClientSocket();
	int receiveData(void *buf, size_t len);
	int sendData(const void *buf, size_t len);
	int connectTo(const std::string &host, const std::string &port);
	void disconnect();
	size_t getSendBufferSize() const;
	size_t getReceiveBufferSize() const;

private:
	size_t sendBufferSize = 0;
	size_t receiveBufferSize = 0;
};

namespace factory {
	ClientSocket makeClientSocket();
}

}
#endif /* SRC_SOCKETS_CLIENTSOCKET_H_ */
