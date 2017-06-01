/*
 * SocketMultiplexer.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXER_H_
#define MULTIPLEXER_H_

#include <thread>
#include <memory>
#include "ClientSocket.h"
#include "MultiplexerImpl.h"

/*
 * AKA Reactor Pattern - https://en.wikipedia.org/wiki/Reactor_pattern
 */

class Multiplexer {
public:
	Multiplexer(MultiplexerImpl *impl) : impl(impl) {
		thread.reset(new std::thread([this](){
			this->impl->multiplex();
		}));
	};

	virtual ~Multiplexer() {};
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
		impl->addClientSocket(std::move(clientSocket));
	};
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::shared_ptr<ClientData> clientData) {
		impl->addClientSocket(std::move(clientSocket), clientData);
	};
	virtual void multiplex() {
		impl->multiplex();
	};
	virtual void cancel() {
		impl->cancel();
	};
	virtual void interrupt() {
		impl->interrupt();
	};
	virtual size_t clientCount() {
		return impl->clientCount();
	};
protected:
	std::shared_ptr<MultiplexerImpl> impl;
	std::unique_ptr<std::thread> thread;
};

#endif /* MULTIPLEXER_H_ */
