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

#include "Factory/SocketFactory.h"

namespace socks {

SocketFactory::SocketFactory(SocketFactoryImpl *impl) : impl(impl) {};

ClientSocket SocketFactory::createClientSocket() {return ClientSocket();};

ClientSocket SocketFactory::createUDPClientSocket() {return ClientSocket(impl->createUDPSocketImpl());};

ClientSocket SocketFactory::createSSLClientSocket() {return ClientSocket(impl->createSSLSocketImpl());};

ServerSocket SocketFactory::createServerSocket() {return ServerSocket();};

ServerSocket SocketFactory::createSSLServerSocket() {return ServerSocket(impl->createSSLSocketImpl());};

Multiplexer SocketFactory::createMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {
	return impl->createMultiplexer(
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback);
};

std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> >
SocketFactory::createSocketPair() {
	return impl->createSocketPair();
};

SocketAddress SocketFactory::createAddress(
		const std::string &host,
		const std::string &port,
		SocketProtocol protocol) {
	return impl->createAddress(host, port, protocol);
};

DatagramSocket SocketFactory::createDatagramSocket() {
	return DatagramSocket();
};

size_t SocketFactory::createID() {
	static std::atomic<std::size_t> id(0);

	return ++id;
}

SocketStream SocketFactory::createSocketStream() {
	return SocketStream();
};

SocketStream SocketFactory::createSSLSocketStream() {
	return SocketStream(std::make_unique<ClientSocket>(createSSLClientSocket()));
};

SocketStream SocketFactory::createUDPSocketStream() {
	return SocketStream(std::make_unique<ClientSocket>(createUDPClientSocket()));
};

SocketFactoryImpl &SocketFactory::getImpl() {
	return *impl;
};

}
