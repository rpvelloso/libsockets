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

#include "Socket/WindowsSocketAddress.h"

namespace socks {

WindowsSocketAddress::WindowsSocketAddress(struct sockaddr* addr,
		size_t addrSize) : SocketAddressImpl(), sockAddrPtr(new struct sockaddr_storage) {
	setSocketAddress(addr, addrSize);
}

WindowsSocketAddress::WindowsSocketAddress(
		const std::string& host,
		const std::string& port,
		SocketProtocol protocol) : SocketAddressImpl(), sockAddrPtr(new struct sockaddr_storage) {

	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = protocol == SocketProtocol::TCP?SOCK_STREAM:SOCK_DGRAM;

	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) == 0) {
		AddrResPtr resPtr(res,freeaddrinfo);
		setSocketAddress(res->ai_addr, res->ai_addrlen);
	}
}

WindowsSocketAddress::~WindowsSocketAddress() {
}

void* WindowsSocketAddress::getSocketAddress() const {
	return reinterpret_cast<void *>(sockAddrPtr.get());
}

void WindowsSocketAddress::setSocketAddressSize(int saSize) {
	static int maxAddrSize = sizeof(struct sockaddr_storage);
	sockAddrSize = std::min(saSize, maxAddrSize);
}

int WindowsSocketAddress::getSocketAddressSize() const {
	return sockAddrSize;
}

std::string WindowsSocketAddress::getHostname() const {
	return hostname;
}

std::string WindowsSocketAddress::getPort() const {
	return port;
}

void WindowsSocketAddress::setSocketAddress(struct sockaddr* addr,
		size_t addrSize) {

	char addrStr[INET_ADDRSTRLEN];

	memcpy(reinterpret_cast<void *>(sockAddrPtr.get()), addr, addrSize);
	sockAddrSize = addrSize;
	inet_ntop(AF_INET, &reinterpret_cast<struct sockaddr_in *>(addr)->sin_addr, addrStr, INET_ADDRSTRLEN);
	hostname = addrStr;
	port = std::to_string(htons(reinterpret_cast<struct sockaddr_in *>(addr)->sin_port));
}

bool WindowsSocketAddress::operator==(const SocketAddressImpl &rhs) {
	if (rhs.getSocketAddressSize() == getSocketAddressSize()) {
		return memcmp(rhs.getSocketAddress(), getSocketAddress(), getSocketAddressSize()) == 0;
	}
	return false;
}

} /* namespace socks */
