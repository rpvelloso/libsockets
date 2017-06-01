/*
 * MultiplexedServer.cpp
 *
 *  Created on: 1 de jun de 2017
 *      Author: rvelloso
 */

#include <iostream>
#include "MultiplexedServer.h"

MultiplexedServer::MultiplexedServer(const std::string& bindAddr, const std::string& port,
		size_t nthreads, MultiplexerCallback callback) : bindAddr(bindAddr), port(port) {

	nthreads = std::max((size_t) 1, nthreads);
	for (size_t i = 0; i < nthreads; ++i)
		multiplexers.emplace_back(socketFactory.CreateMultiplexer(callback));
}

MultiplexedServer::~MultiplexedServer() {
}

void MultiplexedServer::listen() {
	auto serverSocket = socketFactory.CreateServerSocket();
	serverSocket->listenForConnections(bindAddr,port);
	while (true) {
		try {
			auto clientSocket = serverSocket->acceptConnection();
			multiplexers[0]->addClientSocket(std::move(clientSocket), std::make_shared<ClientData>());
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			break;
		}
	}
}
