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
	Socket(SocketImpl *impl) : impl(impl) {};
	virtual ~Socket() {};
protected:
	std::unique_ptr<SocketImpl> impl;
};

#endif /* SOCKET_H_ */
