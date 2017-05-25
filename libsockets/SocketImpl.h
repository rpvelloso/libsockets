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

class ClientSocket;

class SocketImpl {
public:
	SocketImpl() {};
	virtual ~SocketImpl() {};
	virtual int receiveData(void *buf, size_t len) = 0;
	virtual int sendData(const void *buf, size_t len) = 0;
	virtual int connectTo(const std::string &host, const std::string &port) = 0;
	virtual void disconnect() = 0;
	virtual int listenForConnections(const std::string &bindAddr, const std::string &port) = 0;
	virtual ClientSocket *acceptConnection() = 0;
	virtual int setNonBlockingIO(bool status) = 0;
	virtual int reuseAddress() = 0;
};

#endif /* SOCKETIMPL_H_ */