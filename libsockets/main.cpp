/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include <iostream>
#include "Socket.h"
#include "WindowsSocket.h"
#include "ClientSocket.h"
#include "ServerSocket.h"

void testWindowsServerSocket() {
	ServerSocket srv(new WindowsSocket());

	srv.listenForConnections("0.0.0.0","30000");
	std::unique_ptr<ClientSocket> cli(srv.acceptConnection());

	std::cout << "connection received" << std::endl;
	cli->sendData("xpto\n", 5);
	cli->disconnect();
}

void testWindowsClientSocket() {

	ClientSocket cli(new WindowsSocket());

	cli.connectTo("127.0.0.1","30000");
	std::string outp = "hello!\n";

	cli.sendData(outp.c_str(),outp.size());

	char buf[4096];
	int len;
	while ((len = cli.receiveData(static_cast<void *>(buf), 4096)) > 0) {
		buf[len] = 0;
		std::string inp(buf);

		std::cout << inp << std::endl;
	}
}

int main() {
	winSockInit();
	//testWindowsClientSocket();
	testWindowsServerSocket();
	winSockCleanup();
}
