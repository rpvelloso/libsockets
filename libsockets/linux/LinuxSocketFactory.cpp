/*
 * LinuxSocketFactory.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "LinuxSocketFactory.h"
#include "LinuxSocket.h"
#include "OpenSSLSocket.h"
#include "LinuxPoll.h"
#include "LinuxSelect.h"

namespace socks {

SocketFactory socketFactory(new LinuxSocketFactory());

LinuxSocketFactory::LinuxSocketFactory() {
}

LinuxSocketFactory::~LinuxSocketFactory() {
}

ClientSocket LinuxSocketFactory::createClientSocket() {
	return ClientSocket(new LinuxSocket());
}

ClientSocket LinuxSocketFactory::createUDPClientSocket() {
	return ClientSocket(new LinuxSocket(UDPFDFactory));
}

ClientSocket LinuxSocketFactory::createSSLClientSocket() {
	return ClientSocket(new OpenSSLSocket( new LinuxSocket()));
}

ServerSocket LinuxSocketFactory::createServerSocket() {
	return ServerSocket(new LinuxSocket());
}

ServerSocket LinuxSocketFactory::createSSLServerSocket() {
	return ServerSocket(new OpenSSLSocket(new LinuxSocket()));
}

std::unique_ptr<ClientSocket> LinuxSocketFactory::createClientSocketPtr() {
	return std::make_unique<ClientSocket>(new LinuxSocket());
}

std::unique_ptr<ClientSocket> LinuxSocketFactory::createSSLClientSocketPtr() {
	return std::make_unique<ClientSocket>(new OpenSSLSocket(new LinuxSocket()));
}

std::unique_ptr<ServerSocket> LinuxSocketFactory::createServerSocketPtr() {
	return std::make_unique<ServerSocket>(new LinuxSocket);
}

std::unique_ptr<ServerSocket> LinuxSocketFactory::createSSLServerSocketPtr() {
	return std::make_unique<ServerSocket>(new OpenSSLSocket(new LinuxSocket));
}

std::unique_ptr<Multiplexer> LinuxSocketFactory::createMultiplexerPtr(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) {
	return std::make_unique<Multiplexer>(new MultiplexerImpl(
			new LinuxPoll(),
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
	return std::make_pair(std::move(sockIn), std::move(sockOut));
}
}
