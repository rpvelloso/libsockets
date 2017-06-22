/*
 * MultiplexedServer.h
 *
 *  Created on: 1 de jun de 2017
 *      Author: rvelloso
 */

#ifndef MULTIPLEXEDSERVER_H_
#define MULTIPLEXEDSERVER_H_

#include <thread>
#include <vector>
#include <iostream>
#include "SocketFactory.h"

namespace socks {

template <class ClientDataType = ClientData>
class MultiplexedServer {
public:
	MultiplexedServer(const std::string &bindAddr, const std::string &port,
			size_t nthreads, bool secure, 			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback)
 	 	 	 : bindAddr(bindAddr), port(port), secure(secure) {
		nthreads = std::max((size_t) 1, nthreads);
		for (size_t i = 0; i < nthreads; ++i)
			multiplexers.emplace_back(socketFactory.createMultiplexer(
					readCallback,
					connectCallback,
					disconnectCallback,
					writeCallback));
	};

	virtual ~MultiplexedServer() {

	};

	void listen() {
		auto serverSocket = secure?
				socketFactory.createSSLServerSocket():
				socketFactory.createServerSocket();

		serverSocket->listenForConnections(bindAddr, port);

		while (true) {
			try {
				auto clientSocket = serverSocket->acceptConnection();
				std::unique_ptr<ClientData> cliData(new ClientDataType());
				getMultiplexer().addClientSocket(std::move(clientSocket), std::move(cliData));
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				break;
			}
		}
	};
private:
	std::vector<std::unique_ptr<Multiplexer>> multiplexers;
	std::string bindAddr, port;
	bool secure;

	Multiplexer &getMultiplexer() {
		size_t pos = 0;
		size_t min = multiplexers[0]->getClientCount();

		for (size_t i = 1; i < multiplexers.size(); ++i) {
			auto count = multiplexers[i]->getClientCount();
			if (count < min) {
				min = count;
				pos = i;
			}
		}
		return *multiplexers[pos];
	}
};

}
#endif /* MULTIPLEXEDSERVER_H_ */
