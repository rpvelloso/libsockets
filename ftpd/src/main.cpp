/*
 * main.cpp
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#include "libsockets.h"

#include "FTPClient.h"

std::string readline(std::istream &inp) {
	std::string line;

	auto savePos = inp.tellg();
	std::getline(inp, line);
	if (inp && !inp.eof()) {
		std::cerr << "received: " << line << std::endl;
		return line;
	} else {
		inp.clear();
		inp.seekg(savePos);
	}
	return "";
}

int main(int argc, char **argv) {
	auto FTPServer = socks::factory::makeThreadedServer<FTPClient>(
	[](FTPClient &ctx, std::istream &inp, std::ostream &outp) {
		while (inp) {
			auto cmd = readline(inp);
			if (!cmd.empty())
				outp << ctx.processCmd(cmd);
			else
				break;
		}
	},
	[](FTPClient &ctx, std::istream &inp, std::ostream &outp) {
		outp << FTPReplyString[FTPReply::R220];
	});

	FTPServer.listen("127.0.0.1","21");
}


