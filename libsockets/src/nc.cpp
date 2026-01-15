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

#include <iostream>
#include <thread>
#include <vector>
#include "getopt.h"
#include "Factory/SocketFactory.h"
#include "Socket/ClientSocket.h"
#include "Socket/DatagramSocket.h"
#include "Socket/ServerSocket.h"
// TODO uncomment OpenSSLSocket.h and makeSSLClientSocket and makeSSLServerSocket below
// after solving the unresolved externals symbol errors with @openssl bazel module
// #include "Socket/SSL/OpenSSLSocket.h"

class Netcat {
public:
	Netcat(int argc, char **argv) {
		if (parseOptions(argc, argv) != 0)
			return;

		if (verbose) {
			const std::vector<std::string> boolString = {"false", "true"};

			std::cerr << "Parameters: " << std::endl;
			std::cerr << "  host: " << host << ", port: " << port << std::endl;
			std::cerr << "  udp = " << boolString[udp] << std::endl;
			std::cerr << "  secure = " << boolString[secure] << std::endl;
			std::cerr << "  listen = " << boolString[listen] << std::endl;
			std::cerr << "  verbose = " << boolString[verbose] << std::endl;
		}

		if (!listen) {
			auto factory =
					// secure?&socks::factory::makeSSLClientSocket:
					udp?&socks::factory::makeUDPClientSocket:
					&socks::factory::makeClientSocket;

			auto clientSocket = factory();
			client(clientSocket);
		} else {
			if (udp)
				datagramServer();
			else {
				auto factory =
					// secure?&socks::factory::makeSSLServerSocket:
					&socks::factory::makeServerSocket;
				auto serverSocket = factory();
				server(serverSocket);
			}
		}
	}
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
	}

	void client(socks::ClientSocket &clientSocket) {
		if (clientSocket.connectTo(host, port) == 0) {
			sendAndReceive(clientSocket);
		} else
			std::cerr << "error connecting to " << host << ":" << port << std::endl;
	}

	void sendAndReceive(socks::ClientSocket &clientSocket) {
		if (verbose) {
			auto localAddr = clientSocket.getLocalAddress();
			auto remoteAddr = clientSocket.getRemoteAddress();
			std::cerr << "local address " << localAddr.getHostname() << ":" << localAddr.getPort() << std::endl;
			std::cerr << "remote address " << remoteAddr.getHostname() << ":" << remoteAddr.getPort() << std::endl;
		}

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
	}

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
	}

	void usage(const std::string &program) {
		std::cout << "Usage:" << std::endl;
		std::cout <<  program << "[-h host] [-p port] [-u] [-s] [-l]" << std::endl;
		std::cout << "-u UDP socket" << std::endl;
		std::cout << "-s SSL socket" << std::endl;
		std::cout << "-l server socket" << std::endl;
		std::cout << "-v verbose (stderr)" << std::endl;

		std::cout << std::endl;
		std::cout << "(*) -u and -s are mutually exclusive" << std::endl;
	}

	static constexpr size_t bufferSize = 4096;
	std::string host = "", port = "";
	bool
		secure = false,
		udp = false,
		listen = false,
		verbose = false;
};

int main(int argc, char **argv) {
	try {
		Netcat netcat(argc, argv);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
