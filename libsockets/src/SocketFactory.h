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

class SocketFactoryImpl {
public:
	SocketFactoryImpl() {};
	virtual ~SocketFactoryImpl() {};
	virtual SocketImpl *createSocketImpl() = 0;
	virtual SocketImpl *createUDPSocketImpl() = 0;
	virtual SocketImpl *createSSLSocketImpl() = 0;
	virtual Multiplexer createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback) = 0;
	virtual std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() = 0;
	virtual SocketAddress createAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP) = 0;
};

class SocketFactory {
	friend class ClientSocket;
	friend class ServerSocket;
	friend class DatagramSocket;
public:
	SocketFactory() = delete;
	SocketFactory(SocketFactoryImpl *impl) : impl(impl) {};
	ClientSocket createClientSocket() {return ClientSocket();};
	ClientSocket createUDPClientSocket() {return ClientSocket(impl->createUDPSocketImpl());};
	ClientSocket createSSLClientSocket() {return ClientSocket(impl->createSSLSocketImpl());};
	ServerSocket createServerSocket() {return ServerSocket();};
	ServerSocket createSSLServerSocket() {return ServerSocket(impl->createSSLSocketImpl());};
	Multiplexer createMultiplexer(
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
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() {
		return impl->createSocketPair();
	};

	SocketAddress createAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP) {
		return impl->createAddress(host, port, protocol);
	};

	DatagramSocket createDatagramSocket() {
		return DatagramSocket();
	};

	static size_t createID() {
		static std::atomic<std::size_t> id(0);

		return ++id;
	}

	SocketStream createSocketStream() {
		return SocketStream();
	};

	SocketStream createSSLSocketStream() {
		return SocketStream(std::make_unique<ClientSocket>(createSSLClientSocket()));
	};

	SocketStream createUDPSocketStream() {
		return SocketStream(std::make_unique<ClientSocket>(createUDPClientSocket()));
	};

private:
	SocketFactoryImpl &getImpl() {
		return *impl;
	};

	std::unique_ptr<SocketFactoryImpl> impl;
};

extern SocketFactory socketFactory;

}
#endif /* SOCKETFACTORY_H_ */
