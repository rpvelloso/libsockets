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

#include <unistd.h>
#include <cerrno>
#include <exception>
#include "Socket/ClientSocket.h"
#include "Socket/LinuxSocket.h"
#include "Socket/LinuxSocketAddress.h"

#include <string.h>
#include <fcntl.h>

namespace socks {

using ResPtr = std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)>;

LinuxSocket::LinuxSocket(FDFactory &fdFactory) : SocketImpl() {
	fd = fdFactory();
}

std::string LinuxSocket::getPort() {
	return port;
}

LinuxSocket::LinuxSocket(SocketFDType fd) : SocketImpl(fd) {
}

size_t LinuxSocket::getSendBufferSize() {
	int sendBufferSize = 0;
	socklen_t len = sizeof(sendBufferSize);

	if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sendBufferSize, &len) == 0) {
		if (sendBufferSize > 0)
			return sendBufferSize / 2; // half buffer size
	}

	return 0;
}

size_t LinuxSocket::getReceiveBufferSize() {
	int receiveBufferSize = 0;
	socklen_t len = sizeof(receiveBufferSize);

	if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &receiveBufferSize, &len) == 0) {
		if (receiveBufferSize > 0)
			return receiveBufferSize / 2; // half buffer size
	}

	return 0;
}

LinuxSocket::~LinuxSocket() {
}

int LinuxSocket::receiveData(void *buf, size_t len) {
	return recv(fd, static_cast<char *>(buf), len, 0);
}

int LinuxSocket::sendData(const void *buf, size_t len) {
	int ret;

	if ((ret = send(fd, static_cast<const char *>(buf), len, 0)) == -1)
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			throw std::runtime_error("sendData() error: " + std::to_string(errno) + ".");

	return ret;
}

std::pair<int, SocketAddress> LinuxSocket::receiveFrom(void *buf, size_t len) {
	struct sockaddr_storage saddr;
	socklen_t saddrSize = sizeof(saddr);

	auto ret = recvfrom(
			fd,
			reinterpret_cast<char *>(buf),
			len,
			0,
			reinterpret_cast<struct sockaddr*>(&saddr),
			&saddrSize);

	return std::make_pair(ret, SocketAddress(new LinuxSocketAddress(reinterpret_cast<struct sockaddr*>(&saddr), saddrSize)));
};

int LinuxSocket::sendTo(const SocketAddress &addr, const void *buf, size_t len) {
	auto saddr = addr.getSocketAddress();
	auto saddrSize = addr.getSocketAddressSize();
	return sendto(
			fd,
			reinterpret_cast<const char *>(buf),
			len,
			0,
			reinterpret_cast<struct sockaddr*>(saddr),
			saddrSize);
};

int LinuxSocket::connectTo(const std::string &host, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret;

	if ((ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0)
		return ret;

	ResPtr resPtr(res, freeaddrinfo);

	if ((ret = connect(fd, res->ai_addr, res->ai_addrlen)) != 0)
		this->port = port;

	return ret;
}

void LinuxSocket::disconnect() {
	shutdown(fd, SHUT_RDWR);
	close(fd);
}

int LinuxSocket::bindSocket(const std::string &bindAddr, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret;

	if ((ret = getaddrinfo(bindAddr.c_str(), port.c_str(), &hints, &res)) != 0)
		return ret;

	ResPtr resPtr(res,freeaddrinfo);

	if ((ret = bind(fd, res->ai_addr, res->ai_addrlen)) != 0)
		return ret;

	socklen_t len = res->ai_addrlen;
	getsockname(fd, res->ai_addr, &len);
	this->port = std::to_string(htons(((struct sockaddr_in *)res->ai_addr)->sin_port));

	return 0;
}

int LinuxSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
	int ret;

	if ((ret = bindSocket(bindAddr, port)) != 0)
		return ret;

	return listen(fd, 20);
}

std::unique_ptr<SocketImpl> LinuxSocket::acceptConnection() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);

	SocketFDType clientFd = accept(fd, (struct sockaddr *)&addr, &addrlen);

	if (clientFd < 0) {
		throw std::runtime_error("accept() returned an invalid socket. " + std::to_string(errno));
	}

	std::unique_ptr<SocketImpl> ret(new LinuxSocket(clientFd));
	return std::move(ret);
}

int LinuxSocket::setNonBlockingIO(bool status) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return flags;
	flags = status ? (flags|O_NONBLOCK) : (flags&~O_NONBLOCK);
	return fcntl(fd, F_SETFL, flags);
}

int LinuxSocket::reuseAddress() {
	int reuse = 1;
	return setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(int *)&reuse,sizeof(reuse));
}

SocketAddress LinuxSocket::getLocalAddress() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	getsockname(fd, (struct sockaddr *)(&addr), &addrlen);
	return SocketAddress(new LinuxSocketAddress((struct sockaddr *)(&addr), addrlen));
}

SocketAddress LinuxSocket::getRemoteAddress() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	getpeername(fd, (struct sockaddr *)(&addr), &addrlen);
	return SocketAddress(new LinuxSocketAddress((struct sockaddr *)(&addr), addrlen));
}

}
