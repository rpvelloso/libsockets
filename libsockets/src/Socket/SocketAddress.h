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
#include <string>

namespace socks {

class SocketAddressImpl;

enum class SocketProtocol {
	TCP,
	UDP
};

class SocketAddress {
friend class SocketImpl;
public:
	SocketAddress() = delete;
	SocketAddress(SocketAddressImpl *impl);
	SocketAddress(SocketAddress &&);
	~SocketAddress();

	bool operator==(const SocketAddress &rhs);
	bool operator!=(const SocketAddress &rhs);
	std::string getHostname() const;
	std::string getPort() const;
	void *getSocketAddress() const;
	void setSocketAddressSize(int saSize);
	int getSocketAddressSize() const;
protected:
	std::unique_ptr<SocketAddressImpl> impl;
};

namespace factory {
	SocketAddress makeSocketAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP);
}

}

#endif /* SRC_SOCKET_SOCKETADDRESS_H_ */
