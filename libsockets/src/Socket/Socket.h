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

#ifndef SRC_SOCKETS_SOCKET_H_
#define SRC_SOCKETS_SOCKET_H_

#include <memory>

#include "Socket/SocketAddress.h"
#include "Socket/SocketImpl.h"
#include "Socket/SocketState/SocketState.h"

namespace socks {

class Socket {
public:
	Socket(Socket &&) = default;
	virtual Socket &operator=(Socket &&) = default;
	Socket(SocketImpl *impl);
	virtual ~Socket();
	virtual int setNonBlockingIO(bool status);
	virtual std::string getPort();
	virtual SocketImpl &getImpl();
	virtual SocketAddress getLocalAddress();
	virtual SocketAddress getRemoteAddress();
protected:
	std::unique_ptr<SocketImpl> impl;
	std::unique_ptr<SocketState> state;
private:
	Socket() = delete;
};

}

#endif /* SRC_SOCKETS_SOCKET_H_ */
