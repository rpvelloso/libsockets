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

#ifndef WIN_SOCKETS_WINDOWSSOCKETADDRESS_H_
#define WIN_SOCKETS_WINDOWSSOCKETADDRESS_H_

#include "Socket/SocketAddressImpl.h"
#include "Socket/WindowsSocket.h"

namespace socks {

class WindowsSocketAddress : public SocketAddressImpl {
public:
	WindowsSocketAddress(struct sockaddr *addr, size_t addrSize);
	WindowsSocketAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP);
	virtual ~WindowsSocketAddress();

	void *getSocketAddress() const override;
	void setSocketAddressSize(int saSize) override;
	int getSocketAddressSize() const override;
	bool operator==(const SocketAddressImpl &rhs) override;
	std::string getHostname() const override;
	std::string getPort() const override;
private:
	WindowsSocketAddress() = delete;

	void setSocketAddress(struct sockaddr *sa, size_t saSize);

	std::unique_ptr<struct sockaddr_storage> sockAddrPtr;
	int sockAddrSize = sizeof(struct sockaddr_storage);
	std::string hostname, port;
};

} /* namespace socks */

#endif /* WIN_SOCKETS_WINDOWSSOCKETADDRESS_H_ */
