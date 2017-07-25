/*
 * WindowsSocketAddress.cpp
 *
 *  Created on: 21 de jul de 2017
 *      Author: rvelloso
 */

#include "LinuxSocketAddress.h"
#include <string.h>
#include <arpa/inet.h>

namespace socks {

LinuxSocketAddress::LinuxSocketAddress(struct sockaddr* addr,
		size_t addrSize) : sockAddrPtr(new struct sockaddr_storage) {
	setSocketAddress(addr, addrSize);
}

LinuxSocketAddress::LinuxSocketAddress(
		const std::string& host,
		const std::string& port,
		SocketProtocol protocol) : sockAddrPtr(new struct sockaddr_storage) {

	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = protocol == SocketProtocol::TCP?SOCK_STREAM:SOCK_DGRAM;

	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) == 0) {
		AddrResPtr resPtr(res,freeaddrinfo);
		setSocketAddress(res->ai_addr, res->ai_addrlen);
	}
}

LinuxSocketAddress::~LinuxSocketAddress() {
}

void* LinuxSocketAddress::getSocketAddress() const {
	return reinterpret_cast<void *>(sockAddrPtr.get());
}

void LinuxSocketAddress::setSocketAddressSize(int saSize) {
	static int maxAddrSize = sizeof(struct sockaddr_storage);
	sockAddrSize = std::min(saSize, maxAddrSize);
}

int LinuxSocketAddress::getSocketAddressSize() const {
	return sockAddrSize;
}

std::string LinuxSocketAddress::getHostname() const {
	return hostname;
}

std::string LinuxSocketAddress::getPort() const {
	return port;
}

void LinuxSocketAddress::setSocketAddress(struct sockaddr* addr,
		size_t addrSize) {

	char addrStr[INET_ADDRSTRLEN];

	memcpy(reinterpret_cast<void *>(sockAddrPtr.get()), addr, addrSize);
	sockAddrSize = addrSize;
	inet_ntop(AF_INET, &reinterpret_cast<struct sockaddr_in *>(addr)->sin_addr, addrStr, INET_ADDRSTRLEN);
	hostname = addrStr;
	port = std::to_string(htons(reinterpret_cast<struct sockaddr_in *>(addr)->sin_port));
}

bool LinuxSocketAddress::operator==(const SocketAddress &rhs) {
	if (rhs.getSocketAddressSize() == getSocketAddressSize()) {
		return memcmp(rhs.getSocketAddress(), getSocketAddress(), getSocketAddressSize()) == 0;
	}
	return false;
}

} /* namespace socks */
