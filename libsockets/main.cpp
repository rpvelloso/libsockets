/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include <iostream>
#include <memory>
#include <thread>
#include "Socket.h"

#include "SocketFactory.h"

#ifdef _WIN32
#include "WindowsSocket.h"
#endif

void testServerSocket() {
	auto srv = socketFactory->CreateServerSocket();

	srv->listenForConnections("0.0.0.0","30000");
	auto cli = srv->acceptConnection();

	std::cout << "connection received" << std::endl;
	cli->sendData("xpto\n", 5);
	cli->disconnect();
}

void testClientSocket() {

	auto cli = socketFactory->CreateClientSocket();

	cli->connectTo("127.0.0.1","30000");
	std::string outp = "hello!\n";

	cli->sendData(outp.c_str(),outp.size());

	char buf[4096];
	int len;
	while ((len = cli->receiveData(static_cast<void *>(buf), 4096)) > 0) {
		buf[len] = 0;
		std::string inp(buf);

		std::cout << inp << std::endl;
	}
}

void testMultiplexer() {
	std::unique_ptr<Multiplexer> multiplexer = socketFactory->CreateMultiplexer([&multiplexer](std::shared_ptr<ClientSocket> client)->bool {
		char buf[4096];
		int len;

		std::cout << "receiving data. default callback" << std::endl;

		if ((len = client->receiveData(buf, 4096)) <= 0) {
			std::cout << "connection closed." << std::endl;
			return false;
		} else {
			buf[len] = 0x00;
			std::cout << buf << std::endl;
			std::string msg = "data received\n";
			client->sendData(msg.c_str(), msg.size());
			if (std::string(buf).substr(0,9) == "terminate") multiplexer->cancel();
			return true;
		};
	});

	std::thread *server = new std::thread([&multiplexer](){
		auto serverSocket = socketFactory->CreateServerSocket();

		serverSocket->listenForConnections("0.0.0.0","30000");
		while (true) {
			auto clientSocket = serverSocket->acceptConnection();
			std::cout << "connection received" << std::endl;
			if (multiplexer->clientCount() == 0)
				multiplexer->addClientSocket(std::move(clientSocket));
			else
				multiplexer->addClientSocket(std::move(clientSocket),[&multiplexer](std::shared_ptr<ClientSocket> client)->bool {
				char buf[4096];
				int len;

				std::cout << "RECEIVING DATA!!! CUSTOM CALLBACK" << std::endl;

				if ((len = client->receiveData(buf, 4096)) <= 0) {
					std::cout << "connection closed." << std::endl;
					return false;
				} else {
					buf[len] = 0x00;
					std::cout << buf << std::endl;
					std::string msg = "data received\n";
					client->sendData(msg.c_str(), msg.size());
					if (std::string(buf).substr(0,9) == "terminate") multiplexer->cancel();
					return true;
				};
			});
		}
	});

	server->detach();
	std::cout << "multiplexing..." << std::endl;
	multiplexer->multiplex();
	delete server;
	std::cout << "exiting..." << std::endl;
}

int main() {
	winSockInit();
	//testClientSocket();
	//testServerSocket();
	testMultiplexer();
	winSockCleanup();
}
