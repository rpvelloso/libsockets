/*
 * SocketState.h
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SRC_SOCKETSTATE_H_
#define SRC_SOCKETSTATE_H_

#include <exception>
#include "SocketImpl.h"

class SocketState : public SocketImpl {
public:
	SocketState(std::shared_ptr<SocketImpl> impl) : SocketImpl(), impl(impl) {};
	virtual ~SocketState() {};
	int setNonBlockingIO(bool status) override {
		return impl->setNonBlockingIO(status);
	};
	int reuseAddress() override {
		return impl->reuseAddress();
	};
	std::string getPort() override {
		return impl->getPort();
	};
protected:
	std::shared_ptr<SocketImpl> impl;
};

class ConnectedState : public SocketState {
public:
	ConnectedState(std::shared_ptr<SocketImpl> impl) : SocketState(impl) {
		setSocketState(SocketStateType::Connected);
		impl->setSocketState(socketState);
	};
	virtual ~ConnectedState() {};
	int receiveData(void *buf, size_t len) override {
		return impl->receiveData(buf, len);
	};
	int sendData(const void *buf, size_t len) override {
		return impl->sendData(buf, len);
	};
	int connectTo(const std::string &host, const std::string &port) override {
		throw std::runtime_error("invalid operation connectTo()." + std::to_string((int)getSocketState()));
	};
	void disconnect() override {
		impl->disconnect();
	};
	int listenForConnections(const std::string &bindAddr, const std::string &port) override {
		throw std::runtime_error("invalid operation listenForConnections()." + std::to_string((int)getSocketState()));
	};
	std::unique_ptr<ClientSocket> acceptConnection() override {
		throw std::runtime_error("invalid operation listenForConnections()." + std::to_string((int)getSocketState()));
	};
};

class DisconnectedState : public SocketState {
public:
	DisconnectedState(std::shared_ptr<SocketImpl> impl) : SocketState(impl) {
		setSocketState(SocketStateType::Disconnected);
		impl->setSocketState(socketState);
	};
	virtual ~DisconnectedState() {};
	int receiveData(void *buf, size_t len) override {
		throw std::runtime_error("invalid operation receiveData()." + std::to_string((int)getSocketState()));
	};
	int sendData(const void *buf, size_t len) override {
		throw std::runtime_error("invalid operation sendData()." + std::to_string((int)getSocketState()));
	};
	int connectTo(const std::string &host, const std::string &port) override {
		return impl->connectTo(host, port);
	};
	void disconnect() override {
		throw std::runtime_error("invalid operation disconnect()." + std::to_string((int)getSocketState()));
	};
	int listenForConnections(const std::string &bindAddr, const std::string &port) override {
		return impl->listenForConnections(bindAddr, port);
	};
	std::unique_ptr<ClientSocket> acceptConnection() override {
		throw std::runtime_error("invalid operation acceptConnection()." + std::to_string((int)getSocketState()));
	};
};

class ListeningState : public SocketState {
public:
	ListeningState(std::shared_ptr<SocketImpl> impl) : SocketState(impl) {
		setSocketState(SocketStateType::Listening);
		impl->setSocketState(socketState);
	};
	virtual ~ListeningState() {};
	int receiveData(void *buf, size_t len) override {
		throw std::runtime_error("invalid operation receiveData()." + std::to_string((int)getSocketState()));
	};
	int sendData(const void *buf, size_t len) override {
		throw std::runtime_error("invalid operation sendData()." + std::to_string((int)getSocketState()));
	};
	int connectTo(const std::string &host, const std::string &port) override {
		throw std::runtime_error("invalid operation connectTo()." + std::to_string((int)getSocketState()));
	};
	void disconnect() override {
		impl->disconnect();
	};
	int listenForConnections(const std::string &bindAddr, const std::string &port) override {
		throw std::runtime_error("invalid operation listenForConnections()." + std::to_string((int)getSocketState()));
	};
	std::unique_ptr<ClientSocket> acceptConnection() override {
		return impl->acceptConnection();
	};
};

class ClosedState : public SocketState {
public:
	ClosedState(std::shared_ptr<SocketImpl> impl) : SocketState(impl) {
		setSocketState(SocketStateType::Closed);
		impl->setSocketState(socketState);
	};
	virtual ~ClosedState() {};
	int receiveData(void *buf, size_t len) override {
		throw std::runtime_error("invalid operation receiveData()." + std::to_string((int)getSocketState()));
	};
	int sendData(const void *buf, size_t len) override {
		throw std::runtime_error("invalid operation sendData()." + std::to_string((int)getSocketState()));
	};
	int connectTo(const std::string &host, const std::string &port) override {
		throw std::runtime_error("invalid operation connectTo()." + std::to_string((int)getSocketState()));
	};
	void disconnect() override {
		throw std::runtime_error("invalid operation disconnect()." + std::to_string((int)getSocketState()));
	};
	int listenForConnections(const std::string &bindAddr, const std::string &port) override {
		throw std::runtime_error("invalid operation listenForConnections()." + std::to_string((int)getSocketState()));
	};
	std::unique_ptr<ClientSocket> acceptConnection() override {
		throw std::runtime_error("invalid operation acceptConnection()." + std::to_string((int)getSocketState()));
	};
};
#endif /* SRC_SOCKETSTATE_H_ */
