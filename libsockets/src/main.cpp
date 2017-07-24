/*
 * main.cpp
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

/*
 * TODO: UDPSocket interface and sendto, recvfrom impl
 * TODO: refactor: where possible replace unique_ptr<Socket> by move semantics
 * TODO: decide callback format: one for each client or one for the multiplexer
 * TODO: Create class for simple threaded server and clients (one thread per client)
 * TODO: standardize multiplexedClients/Server interfaces and put them inside SocketFactory
 * TODO: implement an FTP and HTTP servers as sample/examples
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
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

void testUDP(const std::string &host, const std::string &port) {
	auto udpSocket = socks::socketFactory.createUDPClientSocket();
	udpSocket.connectTo(host, port);
	while (true) {
		char buf[1024];
		udpSocket.sendData("hello\r\n.", 7);
		auto len = udpSocket.receiveData(buf, 1024);
		if (len > 0) {
			buf[len] = 0;
			std::cout << buf << std::endl;
		}
		auto c = getchar();
		if (c == 'x')
			break;
	}
}

void testDatagram(const std::string &host, const std::string &port) {
	auto dgSocket = socks::socketFactory.createDatagramSocket();
	auto addr = socks::socketFactory.createAddress(host, port);

	dgSocket.bindSocket("0.0.0.0", "");

	std::cout << "starting transmitter..." << std::endl;
	std::thread transmitter([](socks::DatagramSocket &dgSocket, socks::SocketAddress &addr){
		while (true) {
			std::string inp;
			std::getline(std::cin, inp);
			inp.push_back('\n');
			if (dgSocket.sendTo(addr, inp.c_str(), inp.size()) <= 0)
				break;
		}
	}, std::ref(dgSocket), std::ref(addr));
	transmitter.detach();

	std::cout << "starting receiver on port " << dgSocket.getPort() << " ..." << std::endl;
	char buf[4096];
	while (true) {
		auto ret = dgSocket.receiveFrom(buf, 4096);
		if (ret.first <= 0)
			break;

		buf[ret.first] = 0x00;
		std::cout << "received " << ret.first << ": " << buf << std::endl;
	}

}

void testSocketStream(const std::string &host, const std::string &port, bool udp) {
	auto socketStream = udp?
			socks::socketFactory.createUDPSocketStream(host, port):
			socks::socketFactory.createSocketStream(host, port);;

	while (!socketStream.eof()) {
		socketStream << "hello!" << std::endl;
		std::string inp;
		socketStream >> inp;
		std::cout << inp;
	}
}

#include <getopt.h>

class Netcat {
public:
	Netcat(int argc, char **argv) {
		if (parseOptions(argc, argv) != 0)
			return;

		if (verbose) {
			std::cerr << "Parameters: " << std::endl;
			std::cerr << "  host: " << host << ", port: " << port << std::endl;
			std::cerr << "  udp = " << boolString[udp] << std::endl;
			std::cerr << "  secure = " << boolString[secure] << std::endl;
			std::cerr << "  listen = " << boolString[listen] << std::endl;
			std::cerr << "  verbose = " << boolString[verbose] << std::endl;
		}

		if (!listen) {
			if (secure)
				client(socks::socketFactory.createSSLClientSocket());
			else if (udp)
				client(socks::socketFactory.createUDPClientSocket());
			else
				client(socks::socketFactory.createClientSocket());
		} else {
			if (udp)
				datagramServer();
			else {
				if (secure)
					server(socks::socketFactory.createSSLServerSocket());
				else
					server(socks::socketFactory.createServerSocket());
			}
		}
	};
private:
	void server(socks::ServerSocket serverSocket) {
		if (serverSocket.listenForConnections(host, port) == 0) {
			if (verbose)
				std::cerr << "listening for connections on address "
				<< host << ":"
				<< serverSocket.getPort()
				<< std::endl;

			auto clientSocket = serverSocket.acceptConnection();
			serverSocket.disconnect();

			if (verbose)
				std::cerr << "connection received." << std::endl;

			sendAndReceive(std::move(clientSocket));
		} else
			std::cerr << "error listening for connections on "
				<< host << ":" << port << std::endl;
	}

	void datagramServer() {
		auto datagramSocket = socks::socketFactory.createDatagramSocket();
		datagramSocket.bindSocket(host, port);
		char buffer[bufferSize];

		auto datagram = datagramSocket.receiveFrom(buffer, bufferSize);
		auto peer = std::move(datagram.second);

		if (datagram.first <= 0) {
			if (verbose)
				std::cerr << "error receiving datagram." << std::endl;
			return;
		} else {
			if (verbose)
				std::cerr
					<< "connection received from "
					<< peer.getHostname() << ":"
					<< peer.getPort() << std::endl;

			buffer[datagram.first] = 0x00;
			std::cout << buffer;
		}

		auto clientSocket = datagramSocket.makeClientSocket();
		clientSocket.connectTo(peer.getHostname(), peer.getPort());

		sendAndReceive(std::move(clientSocket));
	};

	void client(socks::ClientSocket clientSocket) {
		if (clientSocket.connectTo(host, port) == 0) {
			if (verbose)
				std::cerr << "connected to " << host << ":" << port << std::endl;
			sendAndReceive(std::move(clientSocket));
		} else
			std::cerr << "error connecting to " << host << ":" << port << std::endl;
	}

	void sendAndReceive(socks::ClientSocket clientSocket) {
		std::thread transmitter([](socks::ClientSocket &clientSocket){
			while (true) {
				std::string inp;
				std::getline(std::cin, inp);
				inp.push_back('\n');
				if (clientSocket.sendData(inp.c_str(), inp.size()) <= 0)
					break;
			}
		}, std::ref(clientSocket));
		transmitter.detach();

		// receiver
		char buffer[bufferSize];
		int len;

		while ((len = clientSocket.receiveData(buffer, bufferSize)) > 0) {
			buffer[len] = 0x00;
			std::cout << buffer;
		}

		if (verbose)
			std::cerr << std::endl << "connection terminated." << std::endl;
	};

	int parseOptions(int argc, char **argv) {
		int c;
		while ((c = getopt(argc, argv, "vuslp:h:")) != -1) {
			switch (c) {
			case 'p':
				port = optarg;
				break;
			case 'h':
				host = optarg;
				break;
			case 'u':
				udp = true;
				break;
			case 's':
				secure = true;
				break;
			case 'l':
				listen = true;
				break;
			case 'v':
				verbose = true;
				break;
			default:
				usage(argv[0]);
				return -1;
			}
		}

		if (listen && host == "")
			host = "127.0.0.1";

		if ((udp && secure) || (port == "" && !listen) || (host == "")) {
			usage(argv[0]);
			return -1;
		}
		return 0;
	};

	void usage(const std::string &program) {
		std::cout << "Usage:" << std::endl;
		std::cout <<  program << "[-h host] [-p port] [-u] [-s] [-l]" << std::endl;
		std::cout << "-u UDP socket" << std::endl;
		std::cout << "-s SSL socket" << std::endl;
		std::cout << "-l server socket" << std::endl;
		std::cout << "-v verbose (stderr)" << std::endl;

		std::cout << std::endl;
		std::cout << "(*) -u and -s are mutually exclusive" << std::endl;
	};

	static constexpr size_t bufferSize = 4096;
	static const std::vector<std::string> boolString;
	std::string host = "", port = "";
	bool
		secure = false,
		udp = false,
		listen = false,
		verbose = false;
};

const std::vector<std::string> Netcat::boolString = {"false", "true"};

int main(int argc, char **argv) {
	Netcat netcat(argc, argv);
	return 0;

	try {
//		testDatagram(argv[1], argv[2]);
//		testMultiplexer(std::string(argv[1]) == "ssl");
//		testAsyncClient(argv[1], argv[2], argv[3], std::string(argv[4]) == "ssl");
//		testClient(argv[1], argv[2], std::string(argv[3]) == "ssl");
//		testUDP(argv[1], argv[2]);
//		testSocketStream(argv[1], argv[2], std::string(argv[3]) == "udp");
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
