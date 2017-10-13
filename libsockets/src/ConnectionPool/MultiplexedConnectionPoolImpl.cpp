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

#include "ConnectionPool/MultiplexedConnectionPoolImpl.h"
#include "Factory/SocketFactory.h"

namespace socks {

MultiplexedConnectionPoolImpl::MultiplexedConnectionPoolImpl(
	size_t nthreads,
	MultiplexerCallback readCallback,
	MultiplexerCallback connectCallback,
	MultiplexerCallback disconnectCallback,
	MultiplexerCallback writeCallback)
	 	 : ConnectionPoolImpl() {

	nthreads = std::max((size_t) 1, nthreads);
	for (size_t i = 0; i < nthreads; ++i)
		multiplexers.emplace_back(
				socketFactory.createMultiplexer(
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback)
		);
}

void MultiplexedConnectionPoolImpl::addClientSocket(
		std::unique_ptr<ClientSocket> clientSocket,
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {
	getMultiplexer().addClientSocket(
			std::move(clientSocket),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback);

};

void MultiplexedConnectionPoolImpl::addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
	getMultiplexer().addClientSocket(std::move(clientSocket));
}

MultiplexedConnectionPoolImpl::~MultiplexedConnectionPoolImpl() {
}

Multiplexer &MultiplexedConnectionPoolImpl::getMultiplexer() { // load balance
	size_t pos = 0;
	size_t min = multiplexers[0].getClientCount();

	for (size_t i = 1; i < multiplexers.size(); ++i) {
		auto count = multiplexers[i].getClientCount();
		if (count < min) {
			min = count;
			pos = i;
		}
	}
	return multiplexers[pos];
}

} /* namespace socks */
