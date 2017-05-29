/*
 * SocketFactory.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SOCKETFACTORY_H_
#define SOCKETFACTORY_H_

#include <memory>
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Multiplexer.h"

class SocketFactory {
public:
	SocketFactory() {};
	virtual ~SocketFactory() {};
	virtual std::unique_ptr<ClientSocket> CreateClientSocket() = 0;
	virtual std::unique_ptr<ServerSocket> CreateServerSocket() = 0;
	virtual std::unique_ptr<Multiplexer> CreateMultiplexer(MultiplexerCallback callback) = 0;
};

extern SocketFactory *socketFactory;

#endif /* SOCKETFACTORY_H_ */
