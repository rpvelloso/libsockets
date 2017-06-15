/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

/*
 * TODO: do some more testing with OpenSSLSocket
 * TODO: standardize multiplexedClients/Server interfaces and put them inside SocketFactory
 * TODO: implement an FTP and HTTP servers as sample/examples
 * TODO: Create class for simple threaded server and clients (one thread per client)
 * TODO: standalone streaming client socket
 * TODO: SSL context sharing per site (inside socketFactory)
 */

#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <algorithm>
#include <fstream>
#include "libsockets.h"

void testMultiplexer(bool secure) {
	struct EchoData : public ClientData {
		size_t count=0;
	};

	MultiplexedServer<EchoData> server("0.0.0.0", "30000", 1, secure,
	[](std::istream &inp, std::ostream &outp, ClientData &clientData) {
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

void testAsyncClient(const std::string &host, const std::string &port, const std::string &url, bool secure) {
	class HTTPContext : public ClientData {
	public:
		bool hdr = true;
		std::stringstream headers;
	};

	MultiplexedClients<HTTPContext> clients(1,
	[](std::istream &inp, std::ostream &outp, ClientData &clientData) {
		auto &ctx = static_cast<HTTPContext &>(clientData);

		while (inp.rdbuf()->in_avail() > 0) {
			if (ctx.hdr) {
				std::string line;
				auto savePos = inp.tellg();

				std::getline(inp, line);
				if (inp && !inp.eof()) {
					ctx.headers << line << "\n";
					if (line == "\r")
						ctx.hdr = false;
				} else {
					inp.clear();
					inp.seekg(savePos);
					break;
				}
			} else
				break;
		}
	},
	[host, url](std::istream &inp, std::ostream &outp, ClientData &clientData){
		std::string request = "GET " + url + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
		std::cerr << "sending request" << std::endl << request << std::endl;
		outp << request;
	},
	[](std::istream &inp, std::ostream &outp, ClientData &clientData){
		auto &ctx = static_cast<HTTPContext &>(clientData);
		std::cerr << std::endl << "transaction ended." << std::endl;
		std::cerr << "HDR: " << ctx.headers.str().size() << " bytes" << std::endl;
		std::cerr << ctx.headers.str();
		std::cout << inp.rdbuf();
	});

	if (clients.CreateClientSocket(host, port, secure)) {
		getchar();
	} else
		std::cerr << "error connecting to " << host << ":" << port << std::endl;
}

void testClient(const std::string &host, const std::string &port, bool secure) {
	try {
		auto clientSocket = secure?socketFactory.createSSLClientSocket():socketFactory.createClientSocket();
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
	try {
		//testMultiplexer(std::string(argv[1]) == "ssl");
		testAsyncClient(argv[1], argv[2], argv[3], std::string(argv[4]) == "ssl");
		//testClient(argv[1], argv[2], std::string(argv[3]) == "ssl");
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
