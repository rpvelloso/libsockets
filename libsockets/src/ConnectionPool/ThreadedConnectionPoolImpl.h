/*
 * ThreadedConnectionPoolImpl.h
 *
 *  Created on: 17 de out de 2017
 *      Author: rvelloso
 */

#ifndef SRC_CONNECTIONPOOL_THREADEDCONNECTIONPOOLIMPL_H_
#define SRC_CONNECTIONPOOL_THREADEDCONNECTIONPOOLIMPL_H_

#include "ConnectionPool/ConnectionPoolImpl.h"

namespace socks {

class ThreadedConnectionPoolImpl: public ConnectionPoolImpl {
public:
	ThreadedConnectionPoolImpl() = delete;
	ThreadedConnectionPoolImpl(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback);
	virtual ~ThreadedConnectionPoolImpl();
	void addClientSocket(
			std::unique_ptr<ClientSocket> clientSocket,
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback) override;
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) override;
private:
	MultiplexerCallback readCB, connectCB, disconnectCB, writeCB;
};

} /* namespace socks */

#endif /* SRC_CONNECTIONPOOL_THREADEDCONNECTIONPOOLIMPL_H_ */
