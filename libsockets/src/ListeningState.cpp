/*
 * ListeningState.cpp
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#include "ClientSocket.h"
#include "ListeningState.h"

namespace socks {

ListeningState::ListeningState(SocketImpl &impl) : SocketState(impl) {
	setSocketState(SocketStateType::Listening);
	impl.setSocketState(socketState);
};

ListeningState::~ListeningState() {

};

int ListeningState::receiveData(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveData().");
};

int ListeningState::sendData(const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendData().");
};

int ListeningState::connectTo(const std::string &host, const std::string &port) {
	throw std::runtime_error("invalid operation connectTo().");
};

void ListeningState::disconnect() {
	impl.disconnect();
};

int ListeningState::bindSocket(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation bindSocket().");
};

int ListeningState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation listenForConnections().");
};

std::unique_ptr<SocketImpl> ListeningState::acceptConnection() {
	return impl.acceptConnection();
};

std::string ListeningState::getPort() {
	return impl.getPort();
}
;

size_t ListeningState::getSendBufferSize() {
	throw std::runtime_error("invalid operation getSendBufferSize().");
}

size_t ListeningState::getReceiveBufferSize() {
	throw std::runtime_error("invalid operation getReceiveBufferSize().");
}

}
