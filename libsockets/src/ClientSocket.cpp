/*
 * ClientSocket.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include "ClientSocket.h"

ClientSocket::ClientSocket(std::shared_ptr<SocketImpl> impl) : Socket(impl) {
}

ClientSocket::~ClientSocket() {
}

int ClientSocket::receiveData(void* buf, size_t len) {
	return impl->receiveData(buf, len);
}

int ClientSocket::sendData(const void* buf, size_t len) {
	return impl->sendData(buf, len);
}

int ClientSocket::connectTo(const std::string &host, const std::string &port) {
	return impl->connectTo(host, port);
}

void ClientSocket::disconnect() {
	impl->disconnect();
}
