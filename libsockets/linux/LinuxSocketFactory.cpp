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
#include "LinuxSocketAddress.h"

namespace socks {

SocketFactory socketFactory(new LinuxSocketFactory());

LinuxSocketFactory::LinuxSocketFactory() {
}

LinuxSocketFactory::~LinuxSocketFactory() {
}

SocketImpl *LinuxSocketFactory::createSocketImpl() {
	return new LinuxSocket();
}

SocketImpl *LinuxSocketFactory::createUDPSocketImpl() {
	return new LinuxSocket(UDPFDFactory);
}

SocketImpl *LinuxSocketFactory::createSSLSocketImpl() {
	return new OpenSSLSocket(createSocketImpl());
}

Multiplexer LinuxSocketFactory::createMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) {
	return Multiplexer(new MultiplexerImpl(
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

SocketAddress LinuxSocketFactory::createAddress(
		const std::string& host,
		const std::string& port,
		SocketProtocol protocol) {
	return SocketAddress(new LinuxSocketAddress(host, port, protocol));
}

}
