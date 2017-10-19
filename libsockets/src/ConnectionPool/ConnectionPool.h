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

#ifndef SRC_CONNECTIONPOOL_CONNECTIONPOOL_H_
#define SRC_CONNECTIONPOOL_CONNECTIONPOOL_H_

#include <memory>

#include "ConnectionPool/ConnectionPoolImpl.h"

namespace socks {

class ConnectionPool {
public:
	ConnectionPool(ConnectionPoolImpl *impl);
	void addClientSocket(
			std::unique_ptr<ClientSocket> clientSocket,
			ClientCallback readCallback,
			ClientCallback connectCallback = defaultCallback,
			ClientCallback disconnectCallback = defaultCallback,
			ClientCallback writeCallback = defaultCallback);
private:
	std::unique_ptr<ConnectionPoolImpl> impl;
};

namespace factory {
ConnectionPool makeMultiplexedConnectionPool(size_t numThreads);
ConnectionPool makeThreadedConnectionPool();
}

} /* namespace socks */

#endif /* SRC_CONNECTIONPOOL_CONNECTIONPOOL_H_ */
