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

template <class ClientDataType>
class MultiplexedServer {
public:
	MultiplexedServer(const std::string &bindAddr, const std::string &port,
			size_t nthreads, MultiplexerCallback callback) : bindAddr(bindAddr), port(port) {
		nthreads = std::max((size_t) 1, nthreads);
		for (size_t i = 0; i < nthreads; ++i)
			multiplexers.emplace_back(socketFactory.CreateMultiplexer(callback));
	};
	virtual ~MultiplexedServer() {

	};

	void listen() {
		auto serverSocket = socketFactory.CreateServerSocket();
		serverSocket->listenForConnections(bindAddr,port);
		while (true) {
			try {
				auto clientSocket = serverSocket->acceptConnection();
				multiplexers[0]->addClientSocket(std::move(clientSocket), std::make_shared<ClientDataType>());
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				break;
			}
		}
	};
private:
	std::vector<std::unique_ptr<Multiplexer>> multiplexers;
	std::string bindAddr, port;
};

#endif /* MULTIPLEXEDSERVER_H_ */
