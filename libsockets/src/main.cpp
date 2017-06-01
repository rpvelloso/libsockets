/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

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

void testMultiplexer() {
	struct EchoData : public ClientData {
		size_t count=0;
	};

/*
 * TODO: refactor linux multiplex
 */
	auto multiplexer = socketFactory.CreateMultiplexer(
	[](std::stringstream &inp, std::stringstream &outp, std::shared_ptr<ClientData> clientData) {
		auto echoData = static_cast<EchoData *>(clientData.get());
		size_t bufSize = 4096;
		char buf[4096];
		while (inp.rdbuf()->in_avail() > 0) {
			inp.readsome(buf, bufSize);
			outp.write(buf, inp.gcount());
			echoData->count += inp.gcount();
			//outp << " " << echoData->count;
		}
	});

	auto serverSocket = socketFactory.CreateServerSocket();
	std::thread *server = new std::thread([&multiplexer, &serverSocket](){
		serverSocket->listenForConnections("0.0.0.0","30000");
		while (true) {
			try {
				auto clientSocket = serverSocket->acceptConnection();
				multiplexer->addClientSocket(std::move(clientSocket), std::make_shared<EchoData>());
			} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
				break;
			}
		}
	});

	std::cout << "multiplexing..." << std::endl;
	multiplexer->multiplex();
	serverSocket->disconnect();
	server->join();
	std::cout << "exiting..." << std::endl;
}

int main() {
	winSockInit();
	testMultiplexer();
	winSockCleanup();
}
