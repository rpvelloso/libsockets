/*
 * LinuxSocketFactory.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "LinuxSocketFactory.h"
#include "LinuxSocket.h"
#include "LinuxMultiplexer.h"

SocketFactory *socketFactory = new LinuxSocketFactory();

LinuxSocketFactory::LinuxSocketFactory() {
}

LinuxSocketFactory::~LinuxSocketFactory() {
}

std::unique_ptr<ClientSocket> LinuxSocketFactory::CreateClientSocket() {
	std::shared_ptr<SocketImpl> impl(new LinuxSocket);
	return std::make_unique<ClientSocket>(impl);
}

std::unique_ptr<ServerSocket> LinuxSocketFactory::CreateServerSocket() {
	std::shared_ptr<SocketImpl> impl(new LinuxSocket);
	return std::make_unique<ServerSocket>(impl);
}

std::unique_ptr<Multiplexer> LinuxSocketFactory::CreateMultiplexer(MultiplexerCallback readCallback, MultiplexerCallback writeCallback) {
	return std::make_unique<Multiplexer>(new LinuxMultiplexer(readCallback, writeCallback));
}
