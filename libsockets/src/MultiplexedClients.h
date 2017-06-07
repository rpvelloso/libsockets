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

using ConnectCallbackType = std::function<void(ClientSocket &)>;

template <class ClientDataType = ClientData>
class MultiplexedClients {
public:
	MultiplexedClients(MultiplexerCallback readCallback, ConnectCallbackType connectCallback, size_t nthreads = 1) : connectCallback(connectCallback) {
		nthreads = std::max((size_t) 1, nthreads);
		for (size_t i = 0; i < nthreads; ++i)
			multiplexers.emplace_back(socketFactory.CreateMultiplexer(readCallback));
	};
	virtual ~MultiplexedClients() {

	};

	virtual bool CreateClientSocket(const std::string &host, const std::string &port, bool secure) {
		auto clientSocket = secure?socketFactory.CreateSSLClientSocket():socketFactory.CreateClientSocket();
		auto &multiplexer = getMultiplexer();
		if (clientSocket->connectTo(host, port) == 0) {
			connectCallback(*clientSocket);
			multiplexer.addClientSocket(std::move(clientSocket));
			return true;
		}
		return false;
	}
private:
	std::vector<std::unique_ptr<Multiplexer>> multiplexers;
	ConnectCallbackType connectCallback;

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
