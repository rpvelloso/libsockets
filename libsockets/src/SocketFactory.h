/*
 * SocketFactory.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SOCKETFACTORY_H_
#define SOCKETFACTORY_H_

#include <memory>
#include <atomic>
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Multiplexer.h"

class SocketFactory {
public:
	SocketFactory() {};
	SocketFactory(SocketFactory *impl) : impl(impl) {};
	virtual ~SocketFactory() {};
	virtual std::unique_ptr<ClientSocket> createClientSocket() {return impl->createClientSocket();};
	virtual std::unique_ptr<ClientSocket> createSSLClientSocket() {return impl->createSSLClientSocket();};
	virtual std::unique_ptr<ServerSocket> createServerSocket() {return impl->createServerSocket();};
	virtual std::unique_ptr<ServerSocket> createSSLServerSocket() {return impl->createSSLServerSocket();};
	virtual std::unique_ptr<Multiplexer> createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback) {
		return impl->createMultiplexer(
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback);
	};
	static size_t createID() {
		static std::atomic<std::size_t> id(0);

		return ++id;
	}
private:
	std::unique_ptr<SocketFactory> impl;
};

extern SocketFactory socketFactory;

#endif /* SOCKETFACTORY_H_ */
