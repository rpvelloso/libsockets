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

#ifndef SRC_MULTIPLEXER_MULTIPLEXEDSERVERIMPL_H_
#define SRC_MULTIPLEXER_MULTIPLEXEDSERVERIMPL_H_

#include <thread>
#include <vector>
#include <iostream>

#include "Factory/SocketFactory.h"
#include "Server/ServerImpl.h"
#include "Server/Server.h"

namespace socks {

template <class ClientDataType = ClientData>
class MultiplexedServerImpl : public ServerImpl {
public:
	MultiplexedServerImpl(
			const std::string &bindAddr,
			const std::string &port,
			size_t nthreads,
			std::unique_ptr<ServerSocket> serverSocket,
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback)
 	 	 	 : ServerImpl(), bindAddr(bindAddr), port(port), serverSocket(std::move(serverSocket)) {
		nthreads = std::max((size_t) 1, nthreads);
		for (size_t i = 0; i < nthreads; ++i)
			multiplexers.emplace_back(
					socketFactory.createMultiplexer(
					readCallback,
					connectCallback,
					disconnectCallback,
					writeCallback)
					);
	};

	virtual ~MultiplexedServerImpl() {

	};

	void listen() {
		serverSocket->listenForConnections(bindAddr, port);

		while (true) {
			try {
				getMultiplexer().addClientSocket(
						std::make_unique<ClientSocket>(serverSocket->acceptConnection()),
						std::unique_ptr<ClientData>(new ClientDataType()));
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				break;
			}
		}
	};
private:
	std::vector<Multiplexer> multiplexers;
	std::string bindAddr, port;
	std::unique_ptr<ServerSocket> serverSocket;

	Multiplexer &getMultiplexer() {
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
};

template<class ClientDataType>
Server makeMultiplexedServer(
		const std::string &bindAddr,
		const std::string &port,
		size_t nthreads,
		std::unique_ptr<ServerSocket> serverSocket,
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback = defaultCallback,
		MultiplexerCallback disconnectCallback = defaultCallback,
		MultiplexerCallback writeCallback = defaultCallback) {

	return Server(new MultiplexedServerImpl<ClientDataType>(
			bindAddr,
			port,
			nthreads,
			std::move(serverSocket),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback
	));
}

}
#endif /* SRC_MULTIPLEXER_MULTIPLEXEDSERVERIMPL_H_ */
