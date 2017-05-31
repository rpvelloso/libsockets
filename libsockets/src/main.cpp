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
 * TODO: move stringstream inside MultiplexedClientSocket and deal automatically with I/O
 * changing client callback: the client should only see the I/O streams and his own ClientData.
 */
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
			echoBuffer->ss.write(buf, len);
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
			size_t bufSize = client->getSendBufferSize()*2;
			char buf[bufSize];

			auto savePos = echoBuffer->ss.tellg();
			echoBuffer->ss.readsome(buf, bufSize);
			if (client->sendData(buf, echoBuffer->ss.gcount()) <= 0) {
				echoBuffer->ss.seekg(savePos, echoBuffer->ss.beg);
				break;
			}
		}

		if (echoBuffer->ss.rdbuf()->in_avail() == 0) {
			echoBuffer->ss.str(std::string());
			client->setHasOutput(false);
		} else
			client->setHasOutput(true);
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
