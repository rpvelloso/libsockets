/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

/*
 * TODO: check how SSL works in a threaded environment (need to use 'lock' callbacks)
 * TODO: do more tests with OpenSSLSocket
 * TODO: standardize multiplexedClients/Server and put inside SocketFactory
 * TODO: implement an FTP and HTTP servers as sample/examples
 * TODO: create client ID's in server and allow recovering by ID (factory method do create new ID)
 *       this is needed for chat servers and alike
 * TODO: turn ClientData into a prototype factory???
 * TODO: better refactoring of win/linux classes (specially multiplexer)
 * TODO: simple threaded server (one thread per client)
 * TODO: standalone streaming client socket
 * TODO: SSL context sharing per site (inside socketFactory)
 */

#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <algorithm>
#include "defs.h"
#include "Socket.h"
#include "OpenSSLSocket.h"
#include "SocketFactory.h"
#include "MultiplexedServer.h"
#include "MultiplexedClients.h"

void testMultiplexer(bool secure) {
	struct EchoData : public ClientData {
		size_t count=0;
	};

	MultiplexedServer<EchoData> server("0.0.0.0", "30000", 1, secure,
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
	SSLInit();
	MultiplexedClients<ClientData> clients(
	[](std::stringstream &inp, std::stringstream &outp, ClientData &clientData) {
		size_t bufSize = 4096;
		char buf[4096];
		while (inp.rdbuf()->in_avail() > 0) {
			inp.readsome(buf, bufSize);
			outp.write(buf, inp.gcount());
		}
	});

	if (clients.CreateClientSocket("127.0.0.1","30000", false))
		getchar();
}

void testSSL(const std::string &host, const std::string &port) {
	try {
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
			std::cout << "done." << std::endl;
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}

int main(int argc, char **argv) {
	winSockInit();
	//testMultiplexer(std::string(argv[1]) == "ssl");
	//testAsyncClient();
	testSSL(argv[1], argv[2]);
	winSockCleanup();
}
