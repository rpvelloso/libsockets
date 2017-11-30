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

#include <functional>
#include <map>
#include <atomic>

#include "libsockets.h"
#include "FTPClient.h"
#include "FTPClientInfo.h"
#include "auth/Authentication.h"

class FTPServer {
public:
	FTPServer(bool ssl = false) :
		server(ssl?
			makeSSLFTPServer():
			makeFTPServer()),
		port(ssl?"990":"21") {
		clientCount = 0;
	}

	void start() {
		server.listen("0.0.0.0",port);
	}

	void registerSiteCommand(const std::string &command, SiteCallback callback) {
		std::string cmd(command);
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
		siteCommands[cmd] = callback;
	}

	size_t getClientCount() {
		return clientCount;
	}
private:
	socks::Server server;
	std::string port;
	std::map<std::string, SiteCallback> siteCommands;
	std::atomic<size_t> clientCount;

	static std::string readline(std::istream &inp) {
		std::string line;

		auto savePos = inp.tellg();
		std::getline(inp, line);
		if (inp && !inp.eof()) {
			std::cerr << "< received: " << line << std::endl;
			return line;
		} else
			inp.seekg(savePos);

		return "";
	}

	void onConnect(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
		clientCount++;
		FTPClient &context = ctx.getContext();
		FTPClientInfo &clientInfo = context.getClientInfo();
		clientInfo.setPasvAddr(ctx.getLocalAddress());
		clientInfo.setPeerAddr(ctx.getRemoteAddress());
		clientInfo.setSiteCommands(siteCommands);
		outp << clientInfo.buildReplyString(FTPReply::R220) << std::endl;
	}

	void onDisconnect(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
		clientCount--;
	}

	void onReceive(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
		FTPClient &context = ctx.getContext();
		FTPClientInfo &clientInfo = context.getClientInfo();

		while (inp) {
			auto cmd = readline(inp);
			if (!cmd.empty()) {
				auto reply = context.processCmd(cmd, outp);
				if (reply != FTPReply::RNULL)
					outp << clientInfo.buildReplyString(reply) << std::endl;
			} else
				break;
		}
	}

	socks::Server makeSSLFTPServer() {
		return socks::factory::makeThreadedSSLServer<FTPClient>(
			std::bind(&FTPServer::onReceive, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3),
			std::bind(&FTPServer::onConnect, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3),
			std::bind(&FTPServer::onDisconnect, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3));
	}

	socks::Server makeFTPServer() {
		return socks::factory::makeThreadedServer<FTPClient>(
			std::bind(&FTPServer::onReceive, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3),
			std::bind(&FTPServer::onConnect, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3),
			std::bind(&FTPServer::onDisconnect, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3));
	}
};

AuthenticationFunction FTPClientInfo::authenticate =
[](const std::string &username, const std::string &password, FTPClientInfo& clientInfo) {
	/*
	 * in here a user profile can be loaded into 'clientInfo'
	 * upon authentication in order to define, for example,
	 * a home dir, chroot, etc.
	 */
	return authService.authenticate(username, password);
};

int main(int argc, char **argv) {
	FTPServer ftpServer;

	// SITE CLIENT COUNT
	ftpServer.registerSiteCommand(
		"CLIENT",
		[&ftpServer](const std::string &params, FTPClientInfo &clientInfo) {
			std::stringstream ss(params);
			std::string p1;
			ss >> p1;
			std::transform(p1.begin(), p1.end(), p1.begin(), ::toupper);
			if (p1 == "COUNT")
				return "200 There is/are " + std::to_string(ftpServer.getClientCount()) + " client(s) online.";
			else
				return std::string("501 Invalid SITE CLIENT parameter.");
	});

	ftpServer.start();
}
