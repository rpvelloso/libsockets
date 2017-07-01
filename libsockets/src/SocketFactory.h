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

namespace socks {

class SocketFactory {
public:
	SocketFactory() {};
	SocketFactory(SocketFactory *impl) : impl(impl) {};
	virtual ~SocketFactory() {};
	virtual ClientSocket createClientSocket() {return impl->createClientSocket();};
	virtual ClientSocket createSSLClientSocket() {return impl->createSSLClientSocket();};
	virtual ServerSocket createServerSocket() {return impl->createServerSocket();};
	virtual ServerSocket createSSLServerSocket() {return impl->createSSLServerSocket();};
	virtual std::unique_ptr<ClientSocket> createClientSocketPtr() {return impl->createClientSocketPtr();};
	virtual std::unique_ptr<ClientSocket> createSSLClientSocketPtr() {return impl->createSSLClientSocketPtr();};
	virtual std::unique_ptr<ServerSocket> createServerSocketPtr() {return impl->createServerSocketPtr();};
	virtual std::unique_ptr<ServerSocket> createSSLServerSocketPtr() {return impl->createSSLServerSocketPtr();};
	virtual std::unique_ptr<Multiplexer> createMultiplexerPtr(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback) {
		return impl->createMultiplexerPtr(
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback);
	};
	virtual std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() {
		return impl->createSocketPair();
	};

	static size_t createID() {
		static std::atomic<std::size_t> id(0);

		return ++id;
	}
private:
	std::unique_ptr<SocketFactory> impl;
};

extern SocketFactory socketFactory;

}
#endif /* SOCKETFACTORY_H_ */
