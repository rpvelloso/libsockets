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

#include "defs.h"
#include "Socket/WindowsSocket.h"
#include "Socket/WindowsSocketAddress.h"
#include "Socket/SSL/OpenSSLSocket.h"
#include "Factory/WindowsSocketFactory.h"
#include "Multiplexer/WindowsPoll.h"
#include "Multiplexer/WindowsSelect.h"
#include "Multiplexer/MultiplexerImpl.h"

namespace socks {

SocketFactory socketFactory(new WindowsSocketFactory());

WindowsSocketFactory::WindowsSocketFactory() : SocketFactoryImpl() {
}

WindowsSocketFactory::~WindowsSocketFactory() {
}

SocketImpl *WindowsSocketFactory::createSocketImpl() {
	return new WindowsSocket();
}

SocketImpl *WindowsSocketFactory::createUDPSocketImpl() {
	return new WindowsSocket(UDPFDFactory);
}

SocketImpl *WindowsSocketFactory::createSSLSocketImpl() {
	return new OpenSSLSocket(createSocketImpl());
}

Multiplexer WindowsSocketFactory::createMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) {
	return Multiplexer(new MultiplexerImpl(new WindowsPoll(),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback));
}

std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > WindowsSocketFactory::createSocketPair() {
	/**
	 * Windows alternative to socketpair()
	 */
	ServerSocket server;
	auto sockIn = std::make_unique<ClientSocket>(ClientSocket());
	server.listenForConnections("127.0.0.1",""); // listen on a random free port
	sockIn->connectTo("127.0.0.1",server.getPort());
	sockIn->setNonBlockingIO(true);
	auto sockOut = std::make_unique<ClientSocket>(server.acceptConnection());

	return std::make_pair(std::move(sockIn), std::move(sockOut));
}

SocketAddress WindowsSocketFactory::createAddress(
		const std::string& host,
		const std::string& port,
		SocketProtocol protocol) {
	return SocketAddress(new WindowsSocketAddress(host, port, protocol));
}

}
