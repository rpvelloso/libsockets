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
#include "OpenSSL.h"

namespace socks {
/*
 * AKA Reactor Pattern - https://en.wikipedia.org/wiki/Reactor_pattern
 */

class Multiplexer {
public:
	Multiplexer(MultiplexerImpl *impl) : impl(impl) {
		thread.reset(new std::thread([this](){
			this->impl->multiplex();
			openSSL.threadCleanup();
		}));
	};

	virtual ~Multiplexer() {
		if (impl) {
			impl->cancel();
			thread->join();
		}
	};

	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
		impl->addClientSocket(std::move(clientSocket), std::make_unique<ClientData>());
	};
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			std::unique_ptr<ClientData> clientData) {
		impl->addClientSocket(std::move(clientSocket), std::move(clientData));
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
	virtual size_t getClientCount() {
		return impl->getClientCount();
	};
protected:
	std::unique_ptr<MultiplexerImpl> impl;
	std::unique_ptr<std::thread> thread;
};

}
#endif /* MULTIPLEXER_H_ */
