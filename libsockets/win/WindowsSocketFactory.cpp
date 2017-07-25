/*
 * WindowsSocketFactory.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "defs.h"
#include "WindowsSocket.h"
#include "OpenSSLSocket.h"
#include "WindowsSocketFactory.h"
#include "MultiplexerImpl.h"
#include "WindowsPoll.h"
#include "WindowsSelect.h"
#include "WindowsSocketAddress.h"

namespace socks {

SocketFactory socketFactory(new WindowsSocketFactory());

WindowsSocketFactory::WindowsSocketFactory() : SocketFactory() {
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

ClientSocket WindowsSocketFactory::createClientSocket() {
	return ClientSocket(createSocketImpl());
}

ClientSocket WindowsSocketFactory::createUDPClientSocket() {
	return ClientSocket(createUDPSocketImpl());
}

ClientSocket WindowsSocketFactory::createSSLClientSocket() {
	return ClientSocket(createSSLSocketImpl());
}

ServerSocket WindowsSocketFactory::createServerSocket() {
	return ServerSocket(createSocketImpl());
}

ServerSocket WindowsSocketFactory::createSSLServerSocket() {
	return ServerSocket(createSSLSocketImpl());
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

DatagramSocket WindowsSocketFactory::createDatagramSocket() {
	return DatagramSocket();
}

}
