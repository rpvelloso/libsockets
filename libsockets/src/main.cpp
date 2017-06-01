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

#include "MultiplexedClientSocket.h"

void testMultiplexer() {
	class EchoBuffer : public ClientData {
	public:
		EchoBuffer() : ClientData() {};
		~EchoBuffer() {};
		std::stringstream ss;
	};

/*
 * TODO: write callback really necessary?
 * TODO: refactor multiplex using template pattern implementing hooks with pimpl (win/linux)
 * TODO: hide MultiplexedClientSocket class, pass I/O streams by param to callback
 */
	std::unique_ptr<Multiplexer> multiplexer = socketFactory->CreateMultiplexer(
	[](std::shared_ptr<MultiplexedClientSocket> client) {
		// echo read callback
		auto &inputBuffer = client->getInputBuffer();
		auto &outputBuffer = client->getOutputBuffer();

		size_t bufSize = 4096;
		char buf[4096];
		while (inputBuffer.rdbuf()->in_avail() > 0) {
			inputBuffer.readsome(buf, bufSize);
			outputBuffer.write(buf, inputBuffer.gcount());
		}
	},
	[](std::shared_ptr<MultiplexedClientSocket> client) {
		// empty write callback
	});

	auto serverSocket = socketFactory->CreateServerSocket();
	std::thread *server = new std::thread([&multiplexer, &serverSocket](){
		serverSocket->listenForConnections("0.0.0.0","30000");
		while (true) {
			try {
				auto clientSocket = serverSocket->acceptConnection();
				multiplexer->addClientSocket(std::move(clientSocket), std::make_shared<EchoBuffer>());
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
