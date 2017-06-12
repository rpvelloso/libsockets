/*
 * LinuxSocketFactory.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "LinuxSocketFactory.h"
#include "LinuxSocket.h"
#include "LinuxMultiplexer.h"
#include "OpenSSLSocket.h"

SocketFactory socketFactory(new LinuxSocketFactory());

LinuxSocketFactory::LinuxSocketFactory() {
}

LinuxSocketFactory::~LinuxSocketFactory() {
}

std::unique_ptr<ClientSocket> LinuxSocketFactory::CreateClientSocket() {
	return std::make_unique<ClientSocket>(new LinuxSocket());
}

std::unique_ptr<ClientSocket> LinuxSocketFactory::CreateSSLClientSocket() {
	return std::make_unique<ClientSocket>(new OpenSSLSocket(new LinuxSocket()));
}

std::unique_ptr<ServerSocket> LinuxSocketFactory::CreateServerSocket() {
	return std::make_unique<ServerSocket>(new LinuxSocket);
}

std::unique_ptr<ServerSocket> LinuxSocketFactory::CreateSSLServerSocket() {
	return std::make_unique<ServerSocket>(new OpenSSLSocket(new LinuxSocket));
}

std::unique_ptr<Multiplexer> LinuxSocketFactory::CreateMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) {
	return std::make_unique<Multiplexer>(new MultiplexerImpl( new LinuxPoll(),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback));
}

std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > LinuxSocketFactory::createSocketPair() {
	int selfPipe[2];

	socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, selfPipe);
	auto sockIn = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[0]));
	auto sockOut = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[1]));
	return std::make_pair(sockIn, sockOut);
}
