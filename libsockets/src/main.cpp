/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#include "OpenSSLSocket.h"

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
#include "MultiplexedClients.h"

void testMultiplexer() {
	struct EchoData : public ClientData {
		size_t count=0;
	};

	MultiplexedServer<EchoData> server("0.0.0.0", "30000", 1,
	[](std::stringstream &inp, std::stringstream &outp, ClientData &clientData) {
		/*auto &echoData = static_cast<EchoData &>(clientData);
		size_t bufSize = 4096;
		char buf[4096];*/

		while (inp) {
			std::string cmd;

			auto savePos = inp.tellg();
			std::getline(inp, cmd);
			if (inp && !inp.eof()) {
				std::cout << cmd << std::endl;
			} else {
				inp.clear();
				inp.seekg(savePos);
				break;
			}
		}
		/*while (inp.rdbuf()->in_avail() > 0) {
			inp.readsome(buf, bufSize);
			outp.write(buf, inp.gcount());
			echoData.count += inp.gcount();
			outp << " " << echoData.count << std::endl;
		}*/
	});

	std::cout << "listening..." << std::endl;
	server.listen();
	std::cout << "exiting..." << std::endl;
}

void testAsyncClient() {
	MultiplexedClients<ClientData> clients(
	[](std::stringstream &inp, std::stringstream &outp, ClientData &clientData) {
		size_t bufSize = 4096;
		char buf[4096];
		while (inp.rdbuf()->in_avail() > 0) {
			inp.readsome(buf, bufSize);
			outp.write(buf, inp.gcount());
		}
	});

	if (clients.CreateClientSocket("127.0.0.1","30000"))
		getchar();
}

void testSSL(const std::string &host, const std::string &port) {
	SSLInit();
	auto clientSocket = socketFactory.CreateSSLClientSocket();

	if (clientSocket->connectTo(host, port) == 0) {
		std::string request = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
		std::cout << "sending request: " << request << "to: " << host << ":" << port << std::endl;
		clientSocket->sendData(request.c_str(),request.size());
		std::cout << "request sent. Response: " << std::endl;
		char buf[4096+1];
		int len;
		while ((len=clientSocket->receiveData(buf, 4096)) > 0) {
			buf[len]=0x00;
			std::cout << buf;
		}
	}
}

int main(int argc, char **argv) {
	winSockInit();
	//testMultiplexer();
	//testAsyncClient();
	testSSL(argv[1], argv[2]);
	winSockCleanup();
}
