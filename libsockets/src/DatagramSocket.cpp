/*
 * DatagramSocket.cpp
 *
 *  Created on: 21 de jul de 2017
 *      Author: rvelloso
 */

#include "DatagramSocket.h"
#include "SocketFactory.h"

namespace socks {

DatagramSocket::DatagramSocket(SocketImpl *impl) : Socket(impl) {
}

DatagramSocket::DatagramSocket() : Socket(socketFactory.getImpl().createUDPSocketImpl()) {
}

DatagramSocket::~DatagramSocket() {
}

std::pair<int, SocketAddress> DatagramSocket::receiveFrom(void* buf,
		size_t len) {
	return state->receiveFrom(buf, len);
}

int DatagramSocket::sendTo(const SocketAddress& addr, const void* buf,
		size_t len) {
	return state->sendTo(addr, buf, len);
}

int DatagramSocket::bindSocket(const std::string& bindAddr,
		const std::string& port) {
	return state->bindSocket(bindAddr, port);
}

ClientSocket DatagramSocket::makeClientSocket(const SocketAddress &addr) {
	return makeClientSocket(addr.getHostname(), addr.getPort());
}

ClientSocket DatagramSocket::makeClientSocket(const std::string &host, const std::string &port) {
	if (impl) {
		state.reset();
		auto clientSocket = ClientSocket(impl.release());
		clientSocket.connectTo(host, port);
		return std::move(clientSocket);
	} else
		throw std::runtime_error("invalid operation makeClientSocket()");
}

} /* namespace socks */
