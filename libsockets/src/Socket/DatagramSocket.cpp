/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DatagramSocket.h"
#include "SocketImpl.h"
#include "Factory/SocketFactory.h"

namespace socks {

namespace factory {
	ClientSocket makeUDPClientSocket() {
		return ClientSocket(socketFactory().createUDPSocketImpl());
	}

	DatagramSocket makeDatagramSocket() {
		return DatagramSocket(socketFactory().createUDPSocketImpl());
	}
}

DatagramSocket::DatagramSocket(SocketImpl *impl) : Socket(impl) {
}

DatagramSocket::DatagramSocket() : Socket(socketFactory().createUDPSocketImpl()) {
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
