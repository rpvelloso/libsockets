/*
 * MultiplexedClients.h
 *
 *  Created on: 2 de jun de 2017
 *      Author: roberto
 */

#ifndef MULTIPLEXEDCLIENTS_H_
#define MULTIPLEXEDCLIENTS_H_

#include <thread>
#include <vector>
#include <iostream>
#include "SocketFactory.h"

template <class ClientDataType>
class MultiplexedClients {
public:
	MultiplexedClients(MultiplexerCallback callback, size_t nthreads = 1) {
		nthreads = std::max((size_t) 1, nthreads);
		for (size_t i = 0; i < nthreads; ++i)
			multiplexers.emplace_back(socketFactory.CreateMultiplexer(callback));
	};
	virtual ~MultiplexedClients() {

	};

	virtual bool CreateClientSocket(const std::string host, const std::string port) {
		auto clientSocket = socketFactory.CreateClientSocket();
		auto &multiplexer = getMultiplexer();
		if (clientSocket->connectTo(host, port) == 0) {
			multiplexer.addClientSocket(std::move(clientSocket));
			return true;
		}
		return false;
	}
private:
	std::vector<std::unique_ptr<Multiplexer>> multiplexers;

	Multiplexer &getMultiplexer() {
		size_t pos = 0;
		size_t min = multiplexers[0]->clientCount();

		for (size_t i = 1; i < multiplexers.size(); ++i) {
			auto count = multiplexers[i]->clientCount();
			if (count < min) {
				min = count;
				pos = i;
			}
		}
		return *multiplexers[pos];
	}
};




#endif /* MULTIPLEXEDCLIENTS_H_ */
