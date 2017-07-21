/*
 * DatagramSocket.cpp
 *
 *  Created on: 21 de jul de 2017
 *      Author: rvelloso
 */

#include "DatagramSocket.h"

namespace socks {

DatagramSocket::DatagramSocket(SocketImpl *impl) : Socket(impl) {
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

} /* namespace socks */
