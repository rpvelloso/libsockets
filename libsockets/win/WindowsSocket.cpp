/*
 * WindowsSocket.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include <memory>
#include "ClientSocket.h"
#include "WindowsSocket.h"

using ResPtr = std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)>;

int winSockInit() {
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void winSockCleanup() {
	WSACleanup();
}

WindowsSocket::WindowsSocket() : SocketImpl() {
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	return recv(fd, static_cast<char *>(buf), len, 0);
}

int WindowsSocket::sendData(const void *buf, size_t len) {
	int ret;

	if ((ret = send(fd, static_cast<const char *>(buf), len, 0)) == -1) {
		auto err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
			throw std::runtime_error("sendData() error: " + std::to_string(err) + ".");
	}
	return ret;
}

int WindowsSocket::connectTo(const std::string &host, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret;

	if ((ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0)
		return ret;

	ResPtr resPtr(res, freeaddrinfo);

	if ((ret = connect(fd, res->ai_addr, res->ai_addrlen)) == 0)
		this->port = port;

	return ret;
}

void WindowsSocket::disconnect() {
	shutdown(fd, SD_BOTH);
	closesocket(fd);
}

int WindowsSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
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

	int len = res->ai_addrlen;
	getsockname(fd, res->ai_addr, &len);
	this->port = std::to_string(htons(((struct sockaddr_in *)res->ai_addr)->sin_port));

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
