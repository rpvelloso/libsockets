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
#include <fstream>
#include <getopt.h>
#include "sol.hpp"

#include "libsockets.h"

class LuaServer {
public:
	int parseOptions(int argc, char **argv) {
		int c;
		while ((c = getopt(argc, argv, "vp:h:")) != -1) {
			switch (c) {
			case 'p':
				port = optarg;
				break;
			case 'h':
				host = optarg;
				break;
			case 'v':
				verbose = true;
				break;
			default:
				usage(argv[0]);
				return -1;
			}
		}

		if (host == "")
			host = "127.0.0.1";

		return 0;
	};

	void start() {
		auto server = socks::factory::makeMultiplexedServer(4,
		[](size_t connectionID, std::istream &inp, std::ostream &outp) {
			while (inp) {
				std::string cmd;

				auto savePos = inp.tellg();
				std::getline(inp, cmd);
				if (inp && !inp.eof()) {
					// process line
				} else {
					inp.clear();
					inp.seekg(savePos);
					break;
				}
			}
		});

		std::cout << "listening..." << std::endl;
		server.listen("127.0.0.1", "30000");
		std::cout << "exiting..." << std::endl;
	};
private:
	void usage(const std::string &program) {
		std::cout << "Usage:" << std::endl;
		std::cout <<  program << "[-h host] [-p port] [-v]" << std::endl;
		std::cout << "-v verbose (stderr)" << std::endl << std::endl;
	};

	std::string host = "", port = "";
	bool verbose = false;

};

int main(int argc, char **argv) {
	try {
		LuaServer luaServer;

		luaServer.parseOptions(argc, argv);
		luaServer.start();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
