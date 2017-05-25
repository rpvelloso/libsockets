/*
 * WindowsSocket.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include <unistd.h>
#include "ClientSocket.h"
#include "WindowsSocket.h"

int winSockInit() {
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void winSockCleanup() {
	WSACleanup();
}

WindowsSocket::WindowsSocket() {
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

WindowsSocket::WindowsSocket(SOCKET fd) {
	this->fd = fd;
}

WindowsSocket::~WindowsSocket() {
}

int WindowsSocket::receiveData(void *buf, size_t len) {
	return recv(fd, static_cast<char *>(buf), len, 0);
}

int WindowsSocket::sendData(const void *buf, size_t len) {
	return send(fd, static_cast<const char *>(buf), len, 0);
}

int WindowsSocket::connectTo(const std::string &host, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
	int ret = connect(fd, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);
	return ret;
}

void WindowsSocket::disconnect() {
	shutdown(fd, SD_BOTH);
	close(fd);
}

int WindowsSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(bindAddr.c_str(), port.c_str(), &hints, &res);

	int ret;

	if ((ret = bind(fd, res->ai_addr, res->ai_addrlen)) != 0)
		return ret;
	freeaddrinfo(res);
	return listen(fd, 20);
}

ClientSocket *WindowsSocket::acceptConnection() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);

	SOCKET clientFd = accept(fd, (struct sockaddr *)&addr, &addrlen);
	return new ClientSocket(new WindowsSocket(clientFd));
}

int WindowsSocket::setNonBlockingIO(bool status) {
	unsigned long int mode = status? 1 : 0;
	return ioctlsocket(fd, FIONBIO, &mode);
}

int WindowsSocket::reuseAddress() {
	char reuse = 1;
	return setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(reuse));
}
