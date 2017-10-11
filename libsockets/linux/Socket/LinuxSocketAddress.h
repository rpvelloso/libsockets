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

#ifndef LINUX_SOCKETS_LINUXSOCKETADDRESS_H_
#define LINUX_SOCKETS_LINUXSOCKETADDRESS_H_

#include "Socket/SocketAddress.h"
#include "Socket/LinuxSocket.h"

namespace socks {

class LinuxSocketAddress : public SocketAddress {
public:
	LinuxSocketAddress(LinuxSocketAddress &&) = default;
	virtual LinuxSocketAddress& operator=(LinuxSocketAddress &&) = default;
	LinuxSocketAddress(struct sockaddr *addr, size_t addrSize);
	LinuxSocketAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP);
	virtual ~LinuxSocketAddress();

	void *getSocketAddress() const override;
	void setSocketAddressSize(int saSize) override;
	int getSocketAddressSize() const override;
	bool operator==(const SocketAddress &rhs) override;
	std::string getHostname() const override;
	std::string getPort() const override;
private:
	LinuxSocketAddress() = delete;

	void setSocketAddress(struct sockaddr *sa, size_t saSize);

	std::unique_ptr<struct sockaddr_storage> sockAddrPtr;
	int sockAddrSize = sizeof(struct sockaddr_storage);
	std::string hostname, port;
};

} /* namespace socks */

#endif /* LINUX_SOCKETS_LINUXSOCKETADDRESS_H_ */
