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
	SocketFactory(SocketFactory *impl) : impl(impl) {};
	virtual ~SocketFactory() {};
	virtual std::unique_ptr<ClientSocket> CreateClientSocket() {return impl->CreateClientSocket();};
	virtual std::unique_ptr<ServerSocket> CreateServerSocket() {return impl->CreateServerSocket();};
	virtual std::unique_ptr<Multiplexer> CreateMultiplexer(MultiplexerCallback callback) {
		return impl->CreateMultiplexer(callback);
	};
private:
	std::unique_ptr<SocketFactory> impl;
};

extern SocketFactory socketFactory;

#endif /* SOCKETFACTORY_H_ */
