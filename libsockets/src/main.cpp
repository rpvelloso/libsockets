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


	std::unique_ptr<Multiplexer> multiplexer = socketFactory->CreateMultiplexer(
	[&multiplexer](std::shared_ptr<MultiplexedClientSocket> client)->bool {
		char buf[4096];
		int len;

		if ((len = client->receiveData(buf, 4096)) <= 0) {
			return false;
		} else {
			buf[len] = 0x00;

			/*
			 * business logic goes here
			 */
			EchoBuffer *echoBuffer = static_cast<EchoBuffer *>(client->getClientData().get());
			echoBuffer->ss << buf;
			if (std::string(buf).substr(0,9) == "terminate") multiplexer->cancel();
			/*
			 * end business logic
			 */

			client->setHasOutput(echoBuffer->ss.rdbuf()->in_avail() > 0);
			return true;
		}
	},
	[](std::shared_ptr<MultiplexedClientSocket> client)->bool {
		EchoBuffer *echoBuffer = static_cast<EchoBuffer *>(client->getClientData().get());

		while (echoBuffer->ss.rdbuf()->in_avail() > 0) {
			auto ch = echoBuffer->ss.get();
			if (client->sendData(&ch, sizeof(char)) != 0)
				break;
		}
		client->setHasOutput(echoBuffer->ss.rdbuf()->in_avail() > 0);
		return true;
	}
	);

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
