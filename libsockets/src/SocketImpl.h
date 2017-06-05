/*
 * SocketImpl.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SOCKETIMPL_H_
#define SOCKETIMPL_H_

#include <cstddef>
#include <string>
#include <memory>

class ClientSocket;

enum class SocketStateType {
	Disconnected = 0,
	Connected = 1,
	Listening = 2,
	Closed = 3
};

class SocketImpl {
public:
	SocketImpl() : socketState(SocketStateType::Disconnected) {};
	virtual ~SocketImpl() {};
	virtual int receiveData(void *buf, size_t len) = 0;
	virtual int sendData(const void *buf, size_t len) = 0;
	virtual int connectTo(const std::string &host, const std::string &port) = 0;
	virtual void disconnect() = 0;
	virtual int listenForConnections(const std::string &bindAddr, const std::string &port) = 0;
	virtual std::unique_ptr<ClientSocket> acceptConnection() = 0;
	virtual int setNonBlockingIO(bool status) = 0;
	virtual int reuseAddress() = 0;
	virtual std::string getPort() = 0;
	virtual size_t getSendBufferSize() = 0;
	virtual size_t getReceiveBufferSize() = 0;
	virtual SocketStateType getSocketState() {
		return socketState;
	};
	virtual void setSocketState(SocketStateType socketState) {
		this->socketState = socketState;
	};
protected:
	SocketStateType socketState;
};

template <class SocketImplType, typename SocketFDType>
SocketFDType getFDTemplate(SocketImpl &socket) {
	return static_cast<SocketImplType &>(socket).getFD();
}
#endif /* SOCKETIMPL_H_ */
