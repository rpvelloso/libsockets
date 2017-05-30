/*
 * ClosedState.cpp
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#include "ClosedState.h"

ClosedState::ClosedState(std::shared_ptr<SocketImpl> impl) : SocketState(impl) {
	setSocketState(SocketStateType::Closed);
	impl->setSocketState(socketState);
};

ClosedState::~ClosedState() {

};

int ClosedState::receiveData(void *buf, size_t len) {
	throw std::runtime_error("invalid operation receiveData().");
};

int ClosedState::sendData(const void *buf, size_t len) {
	throw std::runtime_error("invalid operation sendData().");
};

int ClosedState::connectTo(const std::string &host, const std::string &port) {
	throw std::runtime_error("invalid operation connectTo().");
};

void ClosedState::disconnect() {
	throw std::runtime_error("invalid operation disconnect().");
};

int ClosedState::listenForConnections(const std::string &bindAddr, const std::string &port) {
	throw std::runtime_error("invalid operation listenForConnections().");
};

std::unique_ptr<ClientSocket> ClosedState::acceptConnection() {
	throw std::runtime_error("invalid operation acceptConnection().");
};

std::string ClosedState::getPort() {
	throw std::runtime_error("invalid operation getPort().");
};
