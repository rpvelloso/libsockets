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
	return std::make_unique<ClientSocket>(new WindowsSocket);
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::CreateServerSocket() {
	return std::make_unique<ServerSocket>(new WindowsSocket);
}

std::unique_ptr<Multiplexer> WindowsSocketFactory::CreateMultiplexer(std::function<bool(std::shared_ptr<ClientSocket>)> callback) {
	return std::make_unique<Multiplexer>(new WindowsMultiplexer(callback));
}
