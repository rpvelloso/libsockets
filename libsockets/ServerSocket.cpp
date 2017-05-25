/*
 * ServerSocket.cpp
 *
 *  Created on: 25 de mai de 2017
 *      Author: rvelloso
 */

#include "ServerSocket.h"

ServerSocket::ServerSocket(SocketImpl* impl) : Socket(impl) {
}

ServerSocket::~ServerSocket() {
}

int ServerSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
	int ret;
	if ((ret = this->impl->reuseAddress()) != 0)
		return ret;
	return this->impl->listenForConnections(bindAddr, port);
}

ClientSocket *ServerSocket::acceptConnection() {
	return this->impl->acceptConnection();
}
