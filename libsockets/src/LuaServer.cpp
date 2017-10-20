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
#include <thread>
#include <getopt.h>
#include "sol.hpp"

#include "libsockets.h"

class Context {
public:
	size_t id;
};

class LuaServer {
public:
	int parseOptions(int argc, char **argv) {
		int c;
		while ((c = getopt(argc, argv, "vp:h:i:")) != -1) {
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
			case 'i':
				scriptFile = std::string(optarg);
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
		auto server = socks::factory::makeMultiplexedServer<Context>(4,
		[this](Context &ctx, std::istream &inp, std::ostream &outp) {
			while (inp) {
				std::string cmd;

				auto savePos = inp.tellg();
				std::getline(inp, cmd);
				if (inp && !inp.eof()) {
					outp << this->processCmd(ctx, cmd);
				} else {
					inp.clear();
					inp.seekg(savePos);
					break;
				}
			}
		},
		[this](Context &ctx, std::istream &inp, std::ostream &outp) {
			ctx.id = socks::factory::makeID();
		});

		std::thread listenThread([&server, this](){
			server.listen(this->host, this->port);
		});

		std::cout << "listening at " << host << ":" << server.getPort() << std::endl;
		listenThread.join();
		std::cout << "terminating..." << std::endl;
	};
private:
	void usage(const std::string &program) {
		std::cout << "Usage:" << std::endl;
		std::cout <<  program << "[-h host] [-p port] [-v]" << std::endl;
		std::cout << "-v verbose (stderr)" << std::endl << std::endl;
	};

	void bindContext(sol::state &lua) {
		lua.new_usertype<Context>(
			"Context",
			"id",&Context::id);
	};

	std::string processCmd(Context &ctx, const std::string &cmd) {
		sol::state lua;
		lua.open_libraries();

		auto script = lua.load_file(scriptFile);

		if (script.status() == sol::load_status::ok) {
			bindContext(lua);
			script();
		}

		if (script.status() != sol::load_status::ok) {
			std::cout << "Lua error: " << lua_tostring(lua.lua_state(), -1) << std::endl;
			lua_pop(lua.lua_state(), 1);
			throw std::runtime_error("error loading script " + scriptFile);
		}

		std::function<std::string(Context &, std::string)> serverFunc = lua["processCmd"];
		return serverFunc(ctx, cmd);
	};

	std::string host = "", port = "";
	bool verbose = false;
	std::string scriptFile;
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
