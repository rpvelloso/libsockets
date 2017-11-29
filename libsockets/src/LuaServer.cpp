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
#include <mutex>
#include <unordered_map>
#include <queue>
#include <getopt.h>
#include "sol.hpp"

#include "libsockets.h"

class Context {
public:
	~Context() {
		if (id != 0) {
			std::lock_guard<std::mutex> lock(contextLock);
			contextList.erase(id);
		}
	};

	void setID(size_t id) {
		std::lock_guard<std::mutex> lock(contextLock);
		this->id = id;
		contextList[id] = this;
	};

	size_t getID() const {
		return id;
	};

	void initLua(const std::string &scriptFile) {
		lua.open_libraries();

		auto script = lua.load_file(scriptFile);

		if (script.status() == sol::load_status::ok) {
			bindContext();
			script();
		}

		if (script.status() != sol::load_status::ok) {
			std::cout << "Lua error: " << lua_tostring(lua.lua_state(), -1) << std::endl;
			lua_pop(lua.lua_state(), 1);
			throw std::runtime_error("error loading script " + scriptFile);
		}
		serverFunc = lua["processCmd"];
	};

	std::string processCmd(const std::string &cmd) {
		return serverFunc(*this, cmd);
	};

private:
	std::mutex contextLock;
	std::mutex queueLock;
	size_t id = 0;
	static std::unordered_map<size_t, Context *> contextList;
	sol::state lua;
	std::function<std::string(Context &, std::string)> serverFunc;
	std::queue<std::string> messages;

	void bindContext() {
		lua.new_usertype<Context>(
			"Context",
			"getID",&Context::getID);
	};
};

std::unordered_map<size_t, Context *> Context::contextList;

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

		if (scriptFile == "") {
			usage(argv[0]);
			return -1;
		}

		return 0;
	};

	void start() {
		auto server = socks::factory::makeThreadedServer<Context>(//4,
		[](socks::Context<Context> &ctx, std::istream &inp, std::ostream &outp) {
			while (inp) {
				std::string cmd;

				auto savePos = inp.tellg();
				std::getline(inp, cmd);
				if (inp && !inp.eof()) {
					outp << ctx.getContext().processCmd(cmd);
				} else {
					inp.seekg(savePos);
					break;
				}
			}
		},
		[this](socks::Context<Context> &ctx, std::istream &inp, std::ostream &outp) {
			ctx.getContext().setID(this->createID());
			ctx.getContext().initLua(this->scriptFile);
		});

		std::thread listenThread([&server, this](){
			server.listen(this->host, this->port);
		});

		std::cout << "listening at " << host << ":" << server.getPort() << std::endl;
		listenThread.join();
		std::cout << "terminating..." << std::endl;
	};
private:
	size_t createID() {
		static std::atomic<size_t> id(0);
		return ++id;
	};

	void usage(const std::string &program) {
		std::cout << "Usage:" << std::endl;
		std::cout <<  program << "[-h host] [-p port] [-v]" << std::endl;
		std::cout << "-v verbose (stderr)" << std::endl << std::endl;
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
