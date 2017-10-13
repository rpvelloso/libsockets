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

#include <fstream>
#include <iostream>

#include <memory>
#include <atomic>

#include "Factory/SocketFactoryImpl.h"
#include "Socket/ClientSocket.h"
#include "Socket/DatagramSocket.h"
#include "Socket/ServerSocket.h"
#include "Socket/SocketAddress.h"
#include "Socket/SocketStream.h"
#include "Server/Server.h"
#include "ConnectionPool/ConnectionPool.h"

namespace socks {

class SocketFactory {
	friend class ClientSocket;
	friend class ServerSocket;
	friend class DatagramSocket;
public:
	SocketFactory() = delete;
	SocketFactory(SocketFactoryImpl *impl);
	ClientSocket createClientSocket();
	ClientSocket createUDPClientSocket();
	ClientSocket createSSLClientSocket();
	ServerSocket createServerSocket();
	ServerSocket createSSLServerSocket();
	Multiplexer createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback);
	ConnectionPool createMultiplexedConnectionPool(
			size_t numThreads,
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback);
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair();
	SocketAddress createAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP);
	DatagramSocket createDatagramSocket();
	static size_t createID();
	SocketStream createSocketStream();
	SocketStream createSSLSocketStream();
	SocketStream createUDPSocketStream();
	Server createMultiplexedServer(
			size_t numThreads,
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback);
	Server createMultiplexedSSLServer(
			size_t numThreads,
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback);
private:
	SocketFactoryImpl &getImpl();

	std::unique_ptr<SocketFactoryImpl> impl;
};

extern SocketFactory socketFactory;

}

#endif /* SRC_FACTORY_SOCKETFACTORY_H_ */
