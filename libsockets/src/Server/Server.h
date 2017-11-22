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

#include "Socket/BufferedClientSocket.h"
#include "Socket/ClientSocket.h"
#include "Server/ServerImpl.h"
#include "ConnectionPool/MultiplexedConnectionPoolImpl.h"
#include "ConnectionPool/ThreadedConnectionPoolImpl.h"

namespace socks {

class Server {
public:
	Server(ServerImplInterface *impl);
	void listen(const std::string &bindAddr, const std::string &port);
	std::string getPort();
private:
	std::unique_ptr<ServerImplInterface> impl;
};

namespace factory {
	template<class ClientContext>
	Server makeMultiplexedServer(
		size_t numThreads,
		ClientCallback<ClientContext> readCallback,
		ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) {
		return Server(new ServerImpl<ClientContext>(
				new ServerSocket(),
				new ConnectionPool(new MultiplexedConnectionPoolImpl(numThreads)),
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));

	};

	template<class ClientContext>
	Server makeThreadedServer(
		ClientCallback<ClientContext> readCallback,
		ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) {
		return Server(new ServerImpl<ClientContext>(
				new ServerSocket(),
				new ConnectionPool(new ThreadedConnectionPoolImpl()),
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));
	};
}

}
#endif /* SRC_SERVER_SERVER_H_ */
