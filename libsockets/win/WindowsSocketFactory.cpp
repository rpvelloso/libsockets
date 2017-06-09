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
#include "WindowsMultiplexer.h"

SocketFactory socketFactory(new WindowsSocketFactory());

WindowsSocketFactory::WindowsSocketFactory() : SocketFactory() {
}

WindowsSocketFactory::~WindowsSocketFactory() {
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::CreateClientSocket() {
	return std::make_unique<ClientSocket>(new WindowsSocket);
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::CreateSSLClientSocket() {
	return std::make_unique<ClientSocket>(new OpenSSLSocket( new WindowsSocket()));
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::CreateServerSocket() {
	return std::make_unique<ServerSocket>(new WindowsSocket());
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::CreateSSLServerSocket() {
	return std::make_unique<ServerSocket>(new OpenSSLSocket( new WindowsSocket()));
}

std::unique_ptr<Multiplexer> WindowsSocketFactory::CreateMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) {
	return std::make_unique<Multiplexer>(new WindowsMultiplexer(
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback));
}
