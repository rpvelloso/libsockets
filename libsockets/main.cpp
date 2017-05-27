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
	auto multiplexer = socketFactory->CreateMultiplexer([](std::shared_ptr<ClientSocket> client)->bool {
		char buf[4096];
		int len;

		std::cout << "lendo dados" << std::endl;

		if ((len = client->receiveData(buf, 4096)) <= 0) {
			return false;
		} else {
			buf[len] = 0x00;
			std::cout << buf << std::endl;
			client->sendData("dados recebidos\n", 15);
			return true;
		};
	});

	std::thread server([&multiplexer](){
		std::cout << "multiplexing..." << std::endl;
		multiplexer->multiplex();
	});

	auto serverSocket = socketFactory->CreateServerSocket();

	serverSocket->listenForConnections("0.0.0.0","30000");
	while (true) {
		auto clientSocket = serverSocket->acceptConnection();
		std::cout << "conexao recebida" << std::endl;
		multiplexer->addClientSocket(std::move(clientSocket));
	}
}

int main() {
	winSockInit();
	//testClientSocket();
	//testServerSocket();
	testMultiplexer();
	winSockCleanup();
}
