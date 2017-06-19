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

std::unique_ptr<ClientSocket> WindowsSocketFactory::createClientSocket() {
	return std::make_unique<ClientSocket>(new WindowsSocket);
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::createSSLClientSocket() {
	return std::make_unique<ClientSocket>(new OpenSSLSocket( new WindowsSocket()));
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::createServerSocket() {
	return std::make_unique<ServerSocket>(new WindowsSocket());
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::createSSLServerSocket() {
	return std::make_unique<ServerSocket>(new OpenSSLSocket( new WindowsSocket()));
}

std::unique_ptr<Multiplexer> WindowsSocketFactory::createMultiplexer(
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
	auto sockIn = socketFactory.createClientSocket();
	server->listenForConnections("127.0.0.1",""); // listen on a random free port
	sockIn->connectTo("127.0.0.1",server->getPort());
	sockIn->setNonBlockingIO(true);
	auto sockOut = server->acceptConnection();

	return std::make_pair(std::move(sockIn), std::move(sockOut));
}
}
