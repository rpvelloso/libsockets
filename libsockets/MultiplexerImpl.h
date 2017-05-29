/*
 * ClientSocketMultiplexerImpl.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXERIMPL_H_
#define MULTIPLEXERIMPL_H_

#include <memory>
#include "ClientSocket.h"
#include "MultiplexedClientSocket.h"

class MultiplexedClientSocket;

using MultiplexerCallback = std::function<bool(std::shared_ptr<MultiplexedClientSocket>, bool, bool)>;

class MultiplexerImpl {
public:
	MultiplexerImpl(MultiplexerCallback defaultCallback) {
		this->defaultCallback = defaultCallback;
	};
	virtual ~MultiplexerImpl() {};
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) = 0;
	virtual void addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
			MultiplexerCallback customCallback) = 0;
	virtual void multiplex() = 0;
	virtual void cancel() = 0;
	virtual void interrupt() = 0;
	virtual size_t clientCount() = 0;
protected:
	/*
	 * return true: keep multiplexing the client;
	 * return false: remove client from multiplexer.
	 */
	MultiplexerCallback defaultCallback;

	/*
	 * Factory method that wraps a clientSocket with a multiplexing interface
	 */
	std::unique_ptr<MultiplexedClientSocket> makeMultiplexed(std::unique_ptr<ClientSocket> clientSocket) {
		return std::make_unique<MultiplexedClientSocket>(clientSocket->getImpl(), std::bind(&MultiplexerImpl::interrupt, this));
	};
};

#endif /* MULTIPLEXERIMPL_H_ */
