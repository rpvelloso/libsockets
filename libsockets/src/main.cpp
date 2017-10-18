/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * TODO: unit tests
 * TODO: implement an FTP and HTTP servers as sample/examples
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <thread>

#include "libsockets.h"

void testMultiplexer() {
	auto server = socks::factory::makeMultiplexedSSLServer(4,
	[](size_t connectionID, std::istream &inp, std::ostream &outp) {
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
	},
	[](size_t connectionID, std::istream &inp, std::ostream &outp) {
		std::cout << "Client connected. ID: " << connectionID << std::endl;
		outp << "hello " << std::endl;
	},
	[](size_t connectionID, std::istream &inp, std::ostream &outp) {std::cout << "Client disconnected. ID: " << connectionID << std::endl;},
	[](size_t connectionID, std::istream &inp, std::ostream &outp) {std::cout << "sent data to client " << connectionID << std::endl;}
	);

	std::cout << "listening..." << std::endl;
	server.listen("127.0.0.1", "30000");
	std::cout << "exiting..." << std::endl;
}

void testClient(const std::string &host, const std::string &port, bool secure) {
	try {
		auto clientSocket = secure?
				socks::factory::makeSSLClientSocket():
				socks::factory::makeClientSocket();

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
	auto udpSocket = socks::factory::makeUDPClientSocket();
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
	socks::DatagramSocket dgSocket;
	auto addr = socks::factory::makeSocketAddress(host, port);

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
			socks::factory::makeUDPSocketStream():
			socks::factory::makeSocketStream();

	if (socketStream.connectTo(host, port) == 0) {
		while (!socketStream.eof()) {
			socketStream << "hello!" << std::endl;
			std::string inp;
			socketStream >> inp;
			std::cout << inp;
		}
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
			auto factory =
					secure?&socks::factory::makeSSLClientSocket:
					udp?&socks::factory::makeUDPClientSocket:
					&socks::factory::makeClientSocket;

			auto clientSocket = factory();
			client(clientSocket);
		} else {
			if (udp)
				datagramServer();
			else {
				auto factory =
					secure?&socks::factory::makeSSLServerSocket:
					&socks::factory::makeServerSocket;
				auto serverSocket = factory();
				server(serverSocket);
			}
		}
	};
private:
	void server(socks::ServerSocket &serverSocket) {
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

			sendAndReceive(clientSocket);
		} else
			std::cerr << "error listening for connections on "
				<< host << ":" << port << std::endl;
	}

	void datagramServer() {
		socks::DatagramSocket datagramSocket;

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

		auto clientSocket = datagramSocket.makeClientSocket(peer);
		sendAndReceive(clientSocket);
	};

	void client(socks::ClientSocket &clientSocket) {
		if (clientSocket.connectTo(host, port) == 0) {
			if (verbose)
				std::cerr << "connected to " << host << ":" << port << std::endl;
			sendAndReceive(clientSocket);
		} else
			std::cerr << "error connecting to " << host << ":" << port << std::endl;
	}

	void sendAndReceive(socks::ClientSocket &clientSocket) {
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

void testSockAddr() {
	auto addr1 = socks::factory::makeSocketAddress("127.0.0.1", "10000");
	auto addr2 = socks::factory::makeSocketAddress("127.0.0.1", "10000");
	auto addr3 = socks::factory::makeSocketAddress("0.0.0.0", "9000");

	assert(addr1 == addr2); std::cout << "addr1 == addr2 ok" << std::endl;
	assert(addr1 != addr3); std::cout << "addr1 != addr3 ok" << std::endl;
	assert(addr2 != addr3); std::cout << "addr2 != addr3 ok" << std::endl;
}

const std::vector<std::string> Netcat::boolString = {"false", "true"};

int main(int argc, char **argv) {

	try {
		if (argc == 1)
			testMultiplexer();
		else
			Netcat netcat(argc, argv);

		return 0;

//		testDatagram(argv[1], argv[2]);
//		testMultiplexer();
//		testAsyncClient(argv[1], argv[2], argv[3], std::string(argv[4]) == "ssl");
//		testClient(argv[1], argv[2], std::string(argv[3]) == "ssl");
//		testUDP(argv[1], argv[2]);
//		testSocketStream(argv[1], argv[2], std::string(argv[3]) == "udp");
//		testSockAddr();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
