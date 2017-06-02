/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

//TODO: review all shared/unique ptr

#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <algorithm>
#include "Socket.h"

#include "SocketFactory.h"

#ifdef _WIN32
#include "WindowsSocket.h"
#endif

#include "MultiplexedServer.h"

void testMultiplexer() {
	struct EchoData : public ClientData {
		size_t count=0;
	};

	MultiplexedServer<EchoData> server("0.0.0.0", "30000", 4,
	[](std::stringstream &inp, std::stringstream &outp, ClientData &clientData) {
		auto &echoData = static_cast<EchoData &>(clientData);
		size_t bufSize = 4096;
		char buf[4096];
		while (inp.rdbuf()->in_avail() > 0) {
			inp.readsome(buf, bufSize);
			outp.write(buf, inp.gcount());
			echoData.count += inp.gcount();
			//outp << " " << echoData->count;
		}
	});

	std::cout << "listening..." << std::endl;
	server.listen();
	std::cout << "exiting..." << std::endl;
}

int main() {
	winSockInit();
	testMultiplexer();
	winSockCleanup();
}
