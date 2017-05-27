/*
 * WindowsSocket.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include <unistd.h>
#include "ClientSocket.h"
#include "LinuxSocket.h".

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>

using ResPtr = std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)>;

LinuxSocket::LinuxSocket() {
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

LinuxSocket::LinuxSocket(int fd) {
	this->fd = fd;
}

LinuxSocket::~LinuxSocket() {
}

int LinuxSocket::receiveData(void *buf, size_t len) {
	return recv(fd, static_cast<char *>(buf), len, 0);
}

int LinuxSocket::sendData(const void *buf, size_t len) {
	return send(fd, static_cast<const char *>(buf), len, 0);
}

int LinuxSocket::connectTo(const std::string &host, const std::string &port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret;

	if ((ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0)
		return ret;

	ResPtr resPtr(res, freeaddrinfo);

	return connect(fd, res->ai_addr, res->ai_addrlen);
}

void LinuxSocket::disconnect() {
	shutdown(fd, SHUT_RDWR);
	close(fd);
}

int LinuxSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
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

	return listen(fd, 20);
}

std::unique_ptr<ClientSocket> LinuxSocket::acceptConnection() {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);

	int clientFd = accept(fd, (struct sockaddr *)&addr, &addrlen);
	return std::make_unique<ClientSocket>(new LinuxSocket(clientFd));
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