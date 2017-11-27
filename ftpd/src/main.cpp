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

#include "libsockets.h"

#include "FTPClient.h"

AuthenticationFunction FTPClientInfo::authenticate =
[](const std::string &username, const std::string &password, FTPClientInfo& ctx) {
	/*
	 * in here a user profile can be loaded into 'ctx'
	 * upon authentication in order to define, for example,
	 * a home dir, chroot, etc.
	 */
	return true;
};

std::string readline(std::istream &inp) {
	std::string line;

	auto savePos = inp.tellg();
	std::getline(inp, line);
	if (inp && !inp.eof()) {
		std::cerr << "< received: " << line << std::endl;
		return line;
	} else {
		inp.clear();
		inp.seekg(savePos);
	}
	return "";
}

void onConnect(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
	FTPClient &context = ctx.getContext();
	FTPClientInfo &clientInfo = context.getClientInfo();
	clientInfo.setPasvAddr(ctx.getLocalAddress());
	clientInfo.setPeerAddr(ctx.getRemoteAddress());
	outp << context.buildReplyString(FTPReply::R220) << std::endl;
}

void onReceive(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) {
	FTPClient &context = ctx.getContext();
	while (inp) {
		auto cmd = readline(inp);
		if (!cmd.empty()) {
			auto reply = context.processCmd(cmd, outp);
			if (reply != FTPReply::RNULL)
				outp << context.buildReplyString(reply) << std::endl;
		} else
			break;
	}
}

socks::Server makeSSLFTPServer() {
	return socks::factory::makeThreadedSSLServer<FTPClient>(
		onReceive,
		onConnect);
}

socks::Server makeFTPServer() {
	return socks::factory::makeThreadedServer<FTPClient>(
		onReceive,
		onConnect);
}

int main(int argc, char **argv) {
	bool ssl = true;
	std::string port = ssl?"990":"21";
	auto serverFactory = ssl?
		makeSSLFTPServer:
		makeFTPServer;

	auto FTPServer = serverFactory();

	FTPServer.listen("0.0.0.0",port);
}


