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

#include "ConnectionPool/ConnectionPool.h"
#include "ConnectionPool/MultiplexedConnectionPoolImpl.h"
#include "ConnectionPool/ThreadedConnectionPoolImpl.h"

namespace socks {

namespace factory {
	ConnectionPool makeMultiplexedConnectionPool(
		size_t numThreads,
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {

		return ConnectionPool(new MultiplexedConnectionPoolImpl(
				numThreads,
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));

	};

	ConnectionPool makeThreadedConnectionPool(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {

		return ConnectionPool(new ThreadedConnectionPoolImpl(
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));

	};
}

ConnectionPool::ConnectionPool(ConnectionPoolImpl *impl) : impl(impl) {

};

void ConnectionPool::addClientSocket(
		std::unique_ptr<ClientSocket> clientSocket,
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {

	impl->addClientSocket(
			std::move(clientSocket),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback
	);
};

void ConnectionPool::addClientSocket(std::unique_ptr<ClientSocket> clientSocket){
	impl->addClientSocket(std::move(clientSocket));
};

} /* namespace socks */




