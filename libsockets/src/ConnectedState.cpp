/*
 * ConnectedState1.cpp
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#include "ConnectedState.h"

namespace socks {

ConnectedState::ConnectedState(SocketImpl& impl) : SocketState(impl) {
	setSocketState(SocketStateType::Connected);
	impl.setSocketState(socketState);
};

ConnectedState::~ConnectedState() {

};

int ConnectedState::receiveData(void *buf, size_t len) {
	return impl.receiveData(buf, len);
};

int ConnectedState::sendData(const void *buf, size_t len) {
	return impl.sendData(buf, len);
};

int ConnectedState::connectTo(const std::string &host, const std::string &port) {
	throw std::runtime_error("invalid operation connectTo().");
};

void ConnectedState::disconnect() {
	impl.disconnect();
};

int ConnectedState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation listenForConnections().");
};

std::unique_ptr<SocketImpl> ConnectedState::acceptConnection() {
	throw std::runtime_error("invalid operation listenForConnections().");
};

std::string ConnectedState::getPort() {
	return impl.getPort();
}
;

size_t ConnectedState::getSendBufferSize() {
	return impl.getSendBufferSize();
}

size_t ConnectedState::getReceiveBufferSize() {
	return impl.getReceiveBufferSize();
}

}
