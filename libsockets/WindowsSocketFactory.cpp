/*
 * WindowsSocketFactory.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "WindowsSocketFactory.h"
#include "WindowsSocket.h"
#include "WindowsMultiplexer.h"

SocketFactory *socketFactory = new WindowsSocketFactory();

WindowsSocketFactory::WindowsSocketFactory() {
}

WindowsSocketFactory::~WindowsSocketFactory() {
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::CreateClientSocket() {
	std::shared_ptr<SocketImpl> impl(new WindowsSocket);
	return std::make_unique<ClientSocket>(impl);
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::CreateServerSocket() {
	std::shared_ptr<SocketImpl> impl(new WindowsSocket);
	return std::make_unique<ServerSocket>(impl);
}

std::unique_ptr<Multiplexer> WindowsSocketFactory::CreateMultiplexer(MultiplexerCallback callback) {
	return std::make_unique<Multiplexer>(new WindowsMultiplexer(callback));
}
