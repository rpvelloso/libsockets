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

#ifndef SRC_SERVER_SERVERIMPL_H_
#define SRC_SERVER_SERVERIMPL_H_

#include <memory>

#include "Factory/SocketFactory.h"
#include "ConnectionPool/ConnectionPool.h"

namespace socks {

class ServerImpl {
public:
	ServerImpl(
			ServerSocket *serverSocket,
			ConnectionPool *connectionPool) :
			serverSocket(serverSocket),
			connectionPool(connectionPool) {};

	virtual ~ServerImpl() {};
	virtual void listen(const std::string &bindAddr, const std::string &port) {
		serverSocket->listenForConnections(bindAddr, port);

		while (true) {
			try {
				connectionPool->addClientSocket(
						std::make_unique<ClientSocket>(serverSocket->acceptConnection()));
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				break;
			}
		}

	};
private:
	std::unique_ptr<ServerSocket> serverSocket;
	std::unique_ptr<ConnectionPool> connectionPool;
};

}
#endif /* SRC_SERVER_SERVERIMPL_H_ */
