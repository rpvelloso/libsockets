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

#include "FTPServer.h"

FTPServer::FTPServer(bool ssl, bool verbose, const std::string &port) :
	server(ssl?
		makeSSLFTPServer():
		makeFTPServer()),
	verbose(verbose),
	port(ssl?"990":"21") {
	clientCount = 0;
	if (!port.empty())
		this->port = port;

	registerSiteCommand(
		"PIRATE",
		[](const std::string &params, FTPClientInfo &clientInfo) {
			extern std::string pirate;
			return pirate;
	});
	registerSiteCommand(
		"HACK",
		[](const std::string &params, FTPClientInfo &clientInfo) {
			extern std::string hack;
			return hack;
	});
}

void FTPServer::start() {
	std::cout << "Listening on port " << port << std::endl;
	server.listen("0.0.0.0",port);
}

void FTPServer::registerSiteCommand(const std::string &command, SiteCallback callback) {
	std::string cmd(command);
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
	siteCommands[cmd] = callback;
}

size_t FTPServer::getClientCount() {
	return clientCount;
}

std::string FTPServer::readline(std::istream &inp) {
	std::string line;

	auto savePos = inp.tellg();
	std::getline(inp, line);
	if (inp && !inp.eof())
		return line;
	else
		inp.seekg(savePos);

	return "";
}

void FTPServer::onConnect(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
	++clientCount;
	FTPClient &context = ctx.getContext();
	FTPClientInfo &clientInfo = context.getClientInfo();
	clientInfo.setVerbose(verbose);
	clientInfo.setPasvAddr(ctx.getLocalAddress());
	clientInfo.setPeerAddr(ctx.getRemoteAddress());
	clientInfo.setSiteCommands(siteCommands);
	outp << clientInfo.buildReplyString(FTPReply::R220) << std::endl;
}

void FTPServer::onDisconnect(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
	--clientCount;
}

void FTPServer::onReceive(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
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

socks::Server FTPServer::makeSSLFTPServer() {
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

socks::Server FTPServer::makeFTPServer() {
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

std::string pirate =
		"200-                 uuuuuuu\r\n"
		"200-             uu$$$$$$$$$$$uu\r\n"
		"200-          uu$$$$$$$$$$$$$$$$$uu\r\n"
		"200-         u$$$$$$$$$$$$$$$$$$$$$u\r\n"
		"200-        u$$$$$$$$$$$$$$$$$$$$$$$u\r\n"
		"200-       u$$$$$$$$$$$$$$$$$$$$$$$$$u\r\n"
		"200-       u$$$$$$$$$$$$$$$$$$$$$$$$$u\r\n"
		"200-       u$$$$$$\"   \"$$$\"   \"$$$$$$u\r\n"
		"200-       \"$$$$\"      u$u       $$$$\"\r\n"
		"200-        $$$u       u$u       u$$$\r\n"
		"200-        $$$u      u$$$u      u$$$\r\n"
		"200-         \"$$$$uu$$$   $$$uu$$$$\"\r\n"
		"200-          \"$$$$$$$\"   \"$$$$$$$\"\r\n"
		"200-            u$$$$$$$u$$$$$$$u\r\n"
		"200-             u$\"$\"$\"$\"$\"$\"$u\r\n"
		"200-  uuu        $$u$ $ $ $ $u$$       uuu\r\n"
		"200- u$$$$        $$$$$u$u$u$$$       u$$$$\r\n"
		"200-  $$$$$uu      \"$$$$$$$$$\"     uu$$$$$$\r\n"
		"200-u$$$$$$$$$$$uu    \"\"\"\"\"    uuuu$$$$$$$$$$\r\n"
		"200-$$$$\"\"\"$$$$$$$$$$uuu   uu$$$$$$$$$\"\"\"$$$\"\r\n"
		"200- \"\"\"      \"\"$$$$$$$$$$$uu \"\"$\"\"\"\r\n"
		"200-           uuuu \"\"$$$$$$$$$$uuu\r\n"
		"200-  u$$$uuu$$$$$$$$$uu \"\"$$$$$$$$$$$uuu$$$\r\n"
		"200-  $$$$$$$$$$\"\"\"\"           \"\"$$$$$$$$$$$\"\r\n"
		"200-   \"$$$$$\"                      \"\"$$$$\"\"\r\n"
		"200      $$$\"                         $$$$\"";

std::string hack =
		"200-                      ,---.\r\n"
		"200-                     /    |\r\n"
		"200-                    /     |\r\n"
		"200- You shall         /      |\r\n"
		"200-   not pass!      /       |\r\n"
		"200-             ___,'        |\r\n"
		"200-           <  -'          :\r\n"
		"200-            `-.__..--'``-,_\\_\r\n"
		"200-               |o/ <o>` :,.)_`>\r\n"
		"200-               :/ `     ||/)\r\n"
		"200-               (_.).__,-` |\\\r\n"
		"200-               /( `.``   `| :\r\n"
		"200-               \\'`-.)  `  ; ;\r\n"
		"200-               | `       /-<\r\n"
		"200-               |     `  /   `.\r\n"
		"200-,-_-..____     /|  `    :__..-'\\\r\n"
		"200-/,'-.__\\\\  ``-./ :`      ;       \\\r\n"
		"200-`\\ `\\  `\\\\  \\ :  (   `  /  ,   `. \\\r\n"
		"200- \\` \\   \\\\   |  | `   :  :     .\\ \\\r\n"
		"200-  \\ `\\_  ))  :  ;     |  |      ): :\r\n"
		"200- (`-.-'\\ ||  |\\ \\   ` ;  ;       | |\r\n"
		"200-  \\-_   `;;._   ( `  /  /_       | |\r\n"
		"200-   `-.-.// ,'`-._\\__/_,'         ; |\r\n"
		"200-     \\:: :     /     `     ,   /  |\r\n"
		"200-      || |    (        ,' /   /   |\r\n"
		"200       ||                ,'   /    |";
