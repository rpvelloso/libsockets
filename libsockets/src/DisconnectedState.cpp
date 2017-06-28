/*
 * DisconnectedState.cpp
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#include "DisconnectedState.h"

namespace socks {

DisconnectedState::DisconnectedState(SocketImpl &impl) : SocketState(impl) {
	setSocketState(SocketStateType::Disconnected);
	impl.setSocketState(socketState);
};

DisconnectedState::~DisconnectedState() {

};

int DisconnectedState::receiveData(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveData().");
};

int DisconnectedState::sendData(const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendData().");
};

int DisconnectedState::connectTo(const std::string &host, const std::string &port) {
	return impl.connectTo(host, port);
};

void DisconnectedState::disconnect() {
};

int DisconnectedState::bindSocket(const std::string &bindAddr, const std::string &port) {
	return impl.bindSocket(bindAddr, port);
};

int DisconnectedState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	return impl.listenForConnections(bindAddr, port);
};

std::unique_ptr<SocketImpl> DisconnectedState::acceptConnection() {
	throw std::runtime_error("invalid operation acceptConnection().");
};

std::string DisconnectedState::getPort() {
	throw std::runtime_error("invalid operation getPort().");
}
;

size_t DisconnectedState::getSendBufferSize() {
	throw std::runtime_error("invalid operation getSendBufferSize().");
}

size_t DisconnectedState::getReceiveBufferSize() {
	throw std::runtime_error("invalid operation getReceiveBufferSize().");
}

}
