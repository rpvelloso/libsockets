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
#include "factory/FTPSocketFactory.h"
#include "factory/FTPSSLSocketFactory.h"

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

std::unique_ptr<AbstractFTPSocketFactory> ftpSocketFactoryPtr(new FTPSSLSocketFactory());
AbstractFTPSocketFactory &ftpSocketFactory = *ftpSocketFactoryPtr;

int main(int argc, char **argv) {

	auto FTPServer = socks::factory::makeThreadedSSLServer<FTPClient>(
	[](socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) { // onReceive()
		while (inp) {
			auto cmd = readline(inp);
			if (!cmd.empty()) {
				auto reply = ctx.getContext().processCmd(cmd, outp);
				if (reply != FTPReply::RNULL)
					outp << ctx.getContext().buildReplyString(reply) << std::endl;
			} else
				break;
		}
	},
	[](socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp) { // onConnect()
		ctx.getContext().getContext().setPasvAddr(ctx.getLocalAddress());
		ctx.getContext().getContext().setPeerAddr(ctx.getRemoteAddress());
		outp << ctx.getContext().buildReplyString(FTPReply::R220) << std::endl;
	});

	FTPServer.listen("0.0.0.0","990");
	//FTPServer.listen("0.0.0.0","21");
}


