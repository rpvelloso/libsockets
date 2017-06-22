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

namespace socks {

template <class ClientDataType = ClientData>
class MultiplexedClients {
public:
	MultiplexedClients(size_t nthreads,
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback = defaultCallback,
			MultiplexerCallback disconnectCallback = defaultCallback,
			MultiplexerCallback writeCallback = defaultCallback) {
		nthreads = std::max((size_t) 1, nthreads);
		for (size_t i = 0; i < nthreads; ++i)
			multiplexers.emplace_back(socketFactory.createMultiplexer(
					readCallback,
					connectCallback,
					disconnectCallback,
					writeCallback));
	};
	virtual ~MultiplexedClients() {

	};

	virtual bool CreateClientSocket(const std::string &host, const std::string &port, bool secure) {
		auto clientSocket = secure?socketFactory.createSSLClientSocket():socketFactory.createClientSocket();
		if (clientSocket->connectTo(host, port) == 0) {
			std::unique_ptr<ClientData> cliData(new ClientDataType());
			getMultiplexer().addClientSocket(std::move(clientSocket),std::move(cliData));
			return true;
		}
		return false;
	}
private:
	std::vector<std::unique_ptr<Multiplexer>> multiplexers;

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

#endif /* MULTIPLEXEDCLIENTS_H_ */
