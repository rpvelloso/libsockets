/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

/*
 * TODO: refactor: where possible replace unique_ptr<Socket> by move semantics
 * TODO: decide callback format: one for each client or one for the multiplexer
 * TODO: Create class for simple threaded server and clients (one thread per client)
 * TODO: standardize multiplexedClients/Server interfaces and put them inside SocketFactory
 * TODO: implement an FTP and HTTP servers as sample/examples
 */

#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <algorithm>
#include <fstream>
#include "libsockets.h"
//#include "HTTPResponse.h"

void testMultiplexer(bool secure) {
	struct EchoData : public socks::ClientData {
		size_t count=0;
	};

	socks::MultiplexedServer<EchoData> server("0.0.0.0", "30000", 1, secure,
	[](std::istream &inp, std::ostream &outp, socks::ClientData &clientData) {
		/*auto &echoData = static_cast<EchoData &>(clientData);
		size_t bufSize = 4096;
		char buf[4096];*/

		while (inp) {
			std::string cmd;

			auto savePos = inp.tellg();
			std::getline(inp, cmd);
			if (inp && !inp.eof()) {
				if (cmd.back() == '\r')
					cmd.pop_back();

				if (cmd != "") {
					std::fstream file(cmd);
					if (file) {
						std::cout << "file " << cmd << " opened" << std::endl;
						outp << file.rdbuf();
					}
				}
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

/*
void testAsyncClient(const std::string &host, const std::string &port, const std::string &url, bool secure) {
	class HTTPContext : public ClientData {
	public:
		bool hdr = true;
		std::unique_ptr<HTTPResponse> response;
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
					if (line.back() == '\r')
						line.pop_back();
					if (line == "")
						ctx.hdr = false;
					else {
						if (ctx.response.get() == nullptr)
							ctx.response.reset(new HTTPResponse(line));
						else
							ctx.response->addHeader(std::make_unique<HTTPHeader>(line));
					}
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
		for (auto h = ctx.response->begin(); h != ctx.response->end(); ++h)
			std::cout << "\'" << h->first << "\' = \'" << h->second->getValue() << "\'" << std::endl;
		std::cout << inp.rdbuf();
	});

	if (clients.CreateClientSocket(host, port, secure)) {
		getchar();
	} else
		std::cerr << "error connecting to " << host << ":" << port << std::endl;
}
*/
void testClient(const std::string &host, const std::string &port, bool secure) {
	try {
		auto clientSocket = secure?
				socks::socketFactory.createSSLClientSocket():
				socks::socketFactory.createClientSocket();

		if (clientSocket.connectTo(host, port) == 0) {
			std::string request = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
			std::cout << "sending request: " << request << "to: " << host << ":" << port << std::endl;
			clientSocket.sendData(request.c_str(),request.size());
			std::cout << "request sent. Response: " << std::endl;
			char buf[4096+1];
			int len;
			while ((len=clientSocket.receiveData(buf, 4096)) > 0) {
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
//		testMultiplexer(std::string(argv[1]) == "ssl");
//		testAsyncClient(argv[1], argv[2], argv[3], std::string(argv[4]) == "ssl");
		testClient(argv[1], argv[2], std::string(argv[3]) == "ssl");
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
