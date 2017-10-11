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

#ifndef SRC_SOCKET_SOCKETADDRESS_H_
#define SRC_SOCKET_SOCKETADDRESS_H_

#include <memory>

namespace socks {

enum class SocketProtocol {
	TCP,
	UDP
};

class SocketAddress {
public:
	SocketAddress(SocketAddress &&) = default;
	virtual SocketAddress &operator=(SocketAddress &&) = default;
	SocketAddress(SocketAddress *impl) : impl(impl) {};
	virtual ~SocketAddress() {};
	virtual void *getSocketAddress() const { return impl->getSocketAddress(); };
	virtual void setSocketAddressSize(int saSize) { impl->setSocketAddressSize(saSize); };
	virtual int getSocketAddressSize() const { return impl->getSocketAddressSize(); };
	virtual bool operator==(const SocketAddress &rhs) { return impl->operator==(rhs); };
	virtual std::string getHostname() const { return impl->getHostname(); };
	virtual std::string getPort() const { return impl->getPort(); };
protected:
	SocketAddress() {};
	std::unique_ptr<SocketAddress> impl;
};

}

#endif /* SRC_SOCKET_SOCKETADDRESS_H_ */
