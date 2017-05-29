/*
 * Socket.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <memory>
#include "SocketImpl.h"

class Socket {
public:
	Socket(std::shared_ptr<SocketImpl> impl) : impl(impl) {};
	virtual ~Socket() {};
	virtual int setNonBlockingIO(bool status) {
		return impl->setNonBlockingIO(status);
	};
	virtual std::string getPort() {
		return impl->getPort();
	}
	virtual std::shared_ptr<SocketImpl> getImpl() {
		return impl;
	};
protected:
	std::shared_ptr<SocketImpl> impl;
};

#endif /* SOCKET_H_ */
