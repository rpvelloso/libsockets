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

#include "Socket/ServerSocket.h"
#include "Server/Server.h"
#include "ConnectionPool/MultiplexedConnectionPoolImpl.h"
#include "ConnectionPool/ThreadedConnectionPoolImpl.h"

namespace socks {

Server::Server(ServerImpl *impl) : impl(impl) {

};

void Server::listen(const std::string &bindAddr, const std::string &port) {
	impl->listen(bindAddr, port);
};

std::string Server::getPort() {
	return impl->getPort();
};

namespace factory {
	Server makeMultiplexedServer(
			size_t numThreads,
			ClientCallback readCallback,
			ClientCallback connectCallback,
			ClientCallback disconnectCallback,
			ClientCallback writeCallback) {
		return Server(new ServerImpl(
				new ServerSocket(),
				new ConnectionPool(new MultiplexedConnectionPoolImpl(numThreads)),
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));
	};

	Server makeThreadedServer(
			ClientCallback readCallback,
			ClientCallback connectCallback,
			ClientCallback disconnectCallback,
			ClientCallback writeCallback) {
		return Server(new ServerImpl(
				new ServerSocket(),
				new ConnectionPool(new ThreadedConnectionPoolImpl()),
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));
	};
}

}




