/*
 * ServerSocket.cpp
 *
 *  Created on: 25 de mai de 2017
 *      Author: rvelloso
 */

#include "ServerSocket.h"
#include "ClientSocket.h"

ServerSocket::ServerSocket(std::shared_ptr<SocketImpl> impl) : Socket(impl) {
}

ServerSocket::~ServerSocket() {
	impl->disconnect();
}

int ServerSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
	int ret;
	if ((ret = impl->reuseAddress()) != 0) {
		perror("");
		return ret;

	}
	return impl->listenForConnections(bindAddr, port);
}

std::unique_ptr<ClientSocket> ServerSocket::acceptConnection() {
	return impl->acceptConnection();
}
