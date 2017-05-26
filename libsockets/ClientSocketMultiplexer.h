/*
 * SocketMultiplexer.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef CLIENTSOCKETMULTIPLEXER_H_
#define CLIENTSOCKETMULTIPLEXER_H_

#include <memory>
#include "ClientSocket.h"
#include "ClientSocketMultiplexerImpl.h"

class ClientSocketMultiplexer {
public:
	ClientSocketMultiplexer(ClientSocketMultiplexerImpl *impl) : impl(impl) {};
	virtual ~ClientSocketMultiplexer() {};
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
		impl->addClientSocket(clientSocket);
	};
	virtual void multiplex() {
		impl->multiplex();
	};
	virtual void cancel() {
		impl->cancel();
	};
	virtual size_t clientCount() {
		impl->clientCount();
	};
protected:
	std::unique_ptr<ClientSocketMultiplexerImpl> impl;
};

#endif /* CLIENTSOCKETMULTIPLEXER_H_ */
