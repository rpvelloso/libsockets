/*
 * ClientSocketMultiplexerImpl.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXERIMPL_H_
#define MULTIPLEXERIMPL_H_

#include "ClientSocket.h"

using MultiplexerCallback = std::function<bool(std::shared_ptr<ClientSocket>)>;
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
	virtual size_t clientCount() = 0;
protected:
	/*
	 * return true: keep multiplexing the client;
	 * return false: remove client from multiplexer.
	 */
	MultiplexerCallback defaultCallback;
};

#endif /* MULTIPLEXERIMPL_H_ */
