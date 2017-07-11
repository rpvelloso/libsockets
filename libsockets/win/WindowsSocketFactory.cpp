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

namespace socks {

SocketFactory socketFactory(new WindowsSocketFactory());

WindowsSocketFactory::WindowsSocketFactory() : SocketFactory() {
}

WindowsSocketFactory::~WindowsSocketFactory() {
}

ClientSocket WindowsSocketFactory::createClientSocket() {
	return ClientSocket(new WindowsSocket());
}

ClientSocket WindowsSocketFactory::createUDPClientSocket() {
	return ClientSocket(new WindowsSocket(UDPFDFactory));
}

ClientSocket WindowsSocketFactory::createSSLClientSocket() {
	return ClientSocket(new OpenSSLSocket( new WindowsSocket()));
}

ServerSocket WindowsSocketFactory::createServerSocket() {
	return ServerSocket(new WindowsSocket());
}

ServerSocket WindowsSocketFactory::createSSLServerSocket() {
	return ServerSocket(new OpenSSLSocket(new WindowsSocket()));
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::createClientSocketPtr() {
	return std::make_unique<ClientSocket>(new WindowsSocket);
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::createUDPClientSocketPtr() {
	return std::make_unique<ClientSocket>(new WindowsSocket(UDPFDFactory));
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::createSSLClientSocketPtr() {
	return std::make_unique<ClientSocket>(new OpenSSLSocket(new WindowsSocket()));
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::createServerSocketPtr() {
	return std::make_unique<ServerSocket>(new WindowsSocket());
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::createSSLServerSocketPtr() {
	return std::make_unique<ServerSocket>(new OpenSSLSocket(new WindowsSocket()));
}

std::unique_ptr<Multiplexer> WindowsSocketFactory::createMultiplexerPtr(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) {
	return std::make_unique<Multiplexer>(new MultiplexerImpl(new WindowsPoll(),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback));
}

std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > WindowsSocketFactory::createSocketPair() {
	/**
	 * Windows alternative to socketpair()
	 */
	auto server = socketFactory.createServerSocket();
	auto sockIn = socketFactory.createClientSocketPtr();
	server.listenForConnections("127.0.0.1",""); // listen on a random free port
	sockIn->connectTo("127.0.0.1",server.getPort());
	sockIn->setNonBlockingIO(true);
	auto sockOut = server.acceptConnection();

	return std::make_pair(std::move(sockIn), std::move(sockOut));
}
}
