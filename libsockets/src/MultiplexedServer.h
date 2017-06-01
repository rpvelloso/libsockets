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
#include "SocketFactory.h"

class MultiplexedServer {
public:
	MultiplexedServer(const std::string &bindAddr, const std::string &port, size_t nthreads, MultiplexerCallback callback);
	virtual ~MultiplexedServer();
	void listen();
private:
	std::vector<std::unique_ptr<Multiplexer>> multiplexers;
	std::string bindAddr, port;
};

#endif /* MULTIPLEXEDSERVER_H_ */
