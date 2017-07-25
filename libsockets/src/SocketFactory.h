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
#include "SocketStream.h"
#include "Multiplexer.h"
#include "SocketAddress.h"
#include "DatagramSocket.h"

namespace socks {

class SocketFactory {
public:
	SocketFactory() {};
	SocketFactory(SocketFactory *impl) : impl(impl) {};
	virtual ~SocketFactory() {};
	virtual SocketImpl *createSocketImpl() {return impl->createSocketImpl();};
	virtual SocketImpl *createUDPSocketImpl() {return impl->createUDPSocketImpl();};
	virtual SocketImpl *createSSLSocketImpl() {return impl->createSSLSocketImpl();};
	virtual ClientSocket createClientSocket() {return impl->createClientSocket();};
	virtual ClientSocket createUDPClientSocket() {return impl->createUDPClientSocket();};
	virtual ClientSocket createSSLClientSocket() {return impl->createSSLClientSocket();};
	virtual ServerSocket createServerSocket() {return impl->createServerSocket();};
	virtual ServerSocket createSSLServerSocket() {return impl->createSSLServerSocket();};
	virtual Multiplexer createMultiplexer(
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
	virtual std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() {
		return impl->createSocketPair();
	};

	virtual SocketAddress createAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP) {
		return impl->createAddress(host, port, protocol);
	};

	virtual DatagramSocket createDatagramSocket() {
		return impl->createDatagramSocket();
	};

	static size_t createID() {
		static std::atomic<std::size_t> id(0);

		return ++id;
	}

	SocketStream createSocketStream(const std::string &host, const std::string &port) {
		auto clientSocket = std::make_unique<ClientSocket>(impl->createClientSocket());
		clientSocket->connectTo(host, port);
		SocketStream socketStream(std::move(clientSocket));
		return socketStream;
	};

	SocketStream createSSLSocketStream(const std::string &host, const std::string &port) {
		auto clientSocket = std::make_unique<ClientSocket>(impl->createSSLClientSocket());
		clientSocket->connectTo(host, port);
		SocketStream socketStream(std::move(clientSocket));
		return socketStream;
	};

	SocketStream createUDPSocketStream(const std::string &host, const std::string &port) {
		auto clientSocket = std::make_unique<ClientSocket>(impl->createUDPClientSocket());
		clientSocket->connectTo(host, port);
		SocketStream socketStream(std::move(clientSocket));
		return socketStream;
	};

private:
	std::unique_ptr<SocketFactory> impl;
};

extern SocketFactory socketFactory;

}
#endif /* SOCKETFACTORY_H_ */
