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

#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <memory>

#include "Server/ServerImpl.h"
#include "Multiplexer/MultiplexerImpl.h"

namespace socks {

class Server {
public:
	Server(ServerImpl *impl);
	void listen(const std::string &bindAddr, const std::string &port);
private:
	std::unique_ptr<ServerImpl> impl;
};

namespace factory {
	Server makeMultiplexedServer(
		size_t numThreads,
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback);
}

}
#endif /* SRC_SERVER_SERVER_H_ */
