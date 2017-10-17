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

#ifndef SRC_SOCKET_SOCKETADDRESSIMPL_H_
#define SRC_SOCKET_SOCKETADDRESSIMPL_H_

#include <string>

namespace socks {

class SocketAddressImpl {
public:
	SocketAddressImpl() {};
	virtual ~SocketAddressImpl() {};
	virtual bool operator==(const SocketAddressImpl &rhs) = 0;
	virtual std::string getHostname() const = 0;
	virtual std::string getPort() const = 0;
	virtual void *getSocketAddress() const = 0;
	virtual void setSocketAddressSize(int saSize) = 0;
	virtual int getSocketAddressSize() const = 0;
};

} /* namespace socks */

#endif /* SRC_SOCKET_SOCKETADDRESSIMPL_H_ */
