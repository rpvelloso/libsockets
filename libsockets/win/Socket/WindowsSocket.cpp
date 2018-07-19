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

#include <memory>
#include <exception>

#include "Socket/ClientSocket.h"
#include "Socket/WindowsSocket.h"
#include "Socket/WindowsSocketAddress.h"

namespace socks {

class WinSock {
friend class WindowsSocket;
public:
	~WinSock() {
		WSACleanup();
	};
private:
	WinSock() {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw std::runtime_error("error initializing WinSock(): " + std::to_string(WSAGetLastError()));
		}
	};
};

WindowsSocket::WindowsSocket(FDFactory &fdFactory) : SocketImpl() {
	static WinSock winSock;

	fd = fdFactory();
}

std::string WindowsSocket::getPort() {
	return port;
}

size_t WindowsSocket::getSendBufferSize() {
	int sendBufferSize = 0;
	socklen_t len = sizeof(sendBufferSize);

	if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&sendBufferSize, &len) == 0) {
		if (sendBufferSize > 0)
			return sendBufferSize >> 1; // half buffer size
	}

	return 0;
}

size_t WindowsSocket::getReceiveBufferSize() {
	int receiveBufferSize = 0;
	socklen_t len = sizeof(receiveBufferSize);

	if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&receiveBufferSize, &len) == 0) {
		if (receiveBufferSize > 0)
			return receiveBufferSize / 2; // half buffer size
	}

	return 0;
}

WindowsSocket::WindowsSocket(SocketFDType fd) : SocketImpl(fd) {
}

WindowsSocket::~WindowsSocket() {
}

int WindowsSocket::receiveData(void *buf, size_t len) {
	return recv(fd, reinterpret_cast<char *>(buf), len, 0);
}

int WindowsSocket::sendData(const void *buf, size_t len) {
	int ret;

	if ((ret = send(fd, reinterpret_cast<const char *>(buf), len, 0)) == -1) {
		auto err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
			throw std::runtime_error("sendData() error: " + std::to_string(err) + ".");
	}
	return ret;
}

std::pair<int, SocketAddress> WindowsSocket::receiveFrom(void *buf, size_t len) {
	struct sockaddr_storage saddr;
	int saddrSize = sizeof(saddr);

	auto ret = recvfrom(
			fd,
			reinterpret_cast<char *>(buf),
			len,
			0,
			reinterpret_cast<struct sockaddr*>(&saddr),
			&saddrSize);

	return std::make_pair(ret, SocketAddress(new WindowsSocketAddress(reinterpret_cast<struct sockaddr*>(&saddr), saddrSize)));
};

int WindowsSocket::sendTo(const SocketAddress &addr, const void *buf, size_t len) {
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


int WindowsSocket::connectTo(const std::string &host, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret;

	if ((ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0)
		return ret;

	AddrResPtr resPtr(res, freeaddrinfo);

	if ((ret = connect(fd, res->ai_addr, res->ai_addrlen)) == 0)
		this->port = port;

	return ret;
}

void WindowsSocket::disconnect() {
	shutdown(fd, SD_BOTH);
	closesocket(fd);
}

int WindowsSocket::bindSocket(const std::string &bindAddr, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret;

	if ((ret = getaddrinfo(bindAddr.c_str(), port.c_str(), &hints, &res)) != 0)
		return ret;

	AddrResPtr resPtr(res,freeaddrinfo);


	if ((ret = bind(fd, res->ai_addr, res->ai_addrlen)) != 0)
		return ret;

	int len = res->ai_addrlen;
	getsockname(fd, res->ai_addr, &len);
	this->port = std::to_string(htons(((struct sockaddr_in *)res->ai_addr)->sin_port));

	return 0;
}

int WindowsSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
	int ret;

	if ((ret = bindSocket(bindAddr, port)) != 0)
		return ret;

	return listen(fd, 20);
}

std::unique_ptr<SocketImpl> WindowsSocket::acceptConnection() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);

	SocketFDType clientFd = accept(fd, (struct sockaddr *)&addr, &addrlen);

	if (clientFd == INVALID_SOCKET) {
		throw std::runtime_error("accept() returned an invalid socket. " + std::to_string(WSAGetLastError()));
	}

	std::unique_ptr<SocketImpl> ret(new WindowsSocket(clientFd));
	return std::move(ret);
}

int WindowsSocket::setNonBlockingIO(bool status) {
	unsigned long int mode = status? 1 : 0;
	return ioctlsocket(fd, FIONBIO, &mode);
}

int WindowsSocket::reuseAddress() {
	char reuse = 1;
	return setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(reuse));
}


SocketAddress WindowsSocket::getLocalAddress() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	getsockname(fd, (struct sockaddr *)(&addr), &addrlen);
	return SocketAddress(new WindowsSocketAddress((struct sockaddr *)(&addr), addrlen));
}

SocketAddress WindowsSocket::getRemoteAddress() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	getpeername(fd, (struct sockaddr *)(&addr), &addrlen);
	return SocketAddress(new WindowsSocketAddress((struct sockaddr *)(&addr), addrlen));
}

}
