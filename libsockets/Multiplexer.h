/*
 * SocketMultiplexer.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXER_H_
#define MULTIPLEXER_H_

#include <memory>
#include "ClientSocket.h"
#include "MultiplexerImpl.h"

class Multiplexer {
public:
	Multiplexer(MultiplexerImpl *impl) : impl(impl) {};
	virtual ~Multiplexer() {};
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
	std::unique_ptr<MultiplexerImpl> impl;
};

#endif /* MULTIPLEXER_H_ */
