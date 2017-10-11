/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_FACTORY_SOCKETFACTORY_H_
#define SRC_FACTORY_SOCKETFACTORY_H_

#include <memory>
#include <atomic>

#include "Multiplexer/Multiplexer.h"
#include "Socket/ClientSocket.h"
#include "Socket/DatagramSocket.h"
#include "Socket/ServerSocket.h"
#include "Socket/SocketAddress.h"
#include "Socket/SocketStream.h"

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
#endif /* SRC_FACTORY_SOCKETFACTORY_H_ */
