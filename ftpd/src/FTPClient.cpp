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

#include "FTPClient.h"
#include "state/FTPClientConnected.h"
#include "state/FTPClientLoggedIn.h"
#include "state/FTPClientRename.h"
#include "state/FTPClientTransfer.h"
#include "factory/AbstractFTPSocketFactory.h"

#include <sstream>
#include <algorithm>

FTPClient::FTPClient() : state(new FTPClientConnected(context)) {
}

FTPClient::~FTPClient() {
}

FTPReply FTPClient::processCmd(const std::string& cmdline, std::ostream &outp) {
	std::stringstream ss(cmdline);
	std::string command;
	std::string param;

	ss >> command;
	std::getline(ss, param);
	if (!param.empty()) {
		param = param.substr(param.find_first_not_of(' '));
		if (param.back() == '\r')
			param.pop_back();
	}

	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	FTPReply reply;

	if (command == "NOOP" ||
		command == "ALLO")
		reply = state->NOOP();
	else if (command == "QUIT")
		reply = state->QUIT();
	else if (command == "USER")
		reply = state->USER(param);
	else if (command == "PASS") {
		reply = state->PASS(param);
		if (reply == FTPReply::R230)
			state.reset(new FTPClientLoggedIn(context));
	} else if (command == "MODE")
		reply = state->MODE(param);
	else if (command == "STRU")
		reply = state->STRU(param);
	else if (command == "TYPE")
		reply = state->TYPE(param);
	else if (command == "CWD")
		reply = state->CWD(param);
	else if (command == "CDUP")
		reply = state->CWD("..");
	else if (command == "MKD")
		reply = state->MKD(param);
	else if (command == "SIZE")
		reply = state->SIZE(param);
	else if (command == "HELP")
		reply = state->HELP();
	else if (command == "SYST")
		reply = state->SYST();
	else if (
			command == "PWD" ||
			command == "XPWD")
		reply = state->PWD();
	else if (
			command == "DELE" ||
			command == "RMD" ||
			command == "XRMD")
		reply = state->DELE(param);
	else if (command == "RNFR") {
		reply = state->RNFR(param);
		if (reply == FTPReply::R350_RNFR)
			state.reset(new FTPClientRename(context));
	} else if (command == "RNTO") {
		reply = state->RNTO(param);
		if (reply == FTPReply::R250)
			state.reset(new FTPClientLoggedIn(context));
	} else if (command == "PORT") {
		reply = state->PORT(param);
		if (reply == FTPReply::R200) {
			state.reset(new FTPClientTransfer(context, [this]() {
				socks::ClientSocket dataSocket = ftpSocketFactory.makeClientSocket();
				if (dataSocket.connectTo(this->getContext().getAddress(), context.getPort()) != 0)
					throw std::runtime_error("Can't open data connection.");
				return dataSocket;
			}));
		}
	} else if (command == "PASV") {
		reply = state->PASV();
		if (reply == FTPReply::R227) {
			outp << buildReplyString(reply) << std::endl;
			reply = FTPReply::RNULL;
			state.reset(new FTPClientTransfer(context, [this]() {
				auto dataSocket = this->getContext().getPassiveSocket().acceptConnection();
				if (this->getContext().getPeerAddr() != dataSocket.getRemoteAddress().getHostname())
					throw std::runtime_error("Invalid peer address.");
				context.getPassiveSocket().disconnect();
				return dataSocket;
			}));
		}
	} else if (
			command == "LIST" ||
			command == "NLST") {
		outp << buildReplyString(FTPReply::R150) << std::endl;
		reply = state->LIST(param);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "RETR") {
		outp << buildReplyString(FTPReply::R150) << std::endl;
		reply = state->RETR(param);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "STOR") {
		outp << buildReplyString(FTPReply::R150) << std::endl;
		reply = state->STOR(param);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "APPE") {
		outp << buildReplyString(FTPReply::R150) << std::endl;
		reply = state->APPE(param);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "REST")
		reply = state->REST(param);
	else
		reply = FTPReply::R500;

	return reply;
}

std::string FTPClient::buildReplyString(FTPReply reply) {
	if (reply == FTPReply::RNULL)
		throw std::runtime_error("invalid FTP reply");

	std::string replyStr = FTPReplyString[reply];

	switch (reply) {
	case FTPReply::R200_TYPE:
		replyStr += context.getType();
		break;
	case FTPReply::R213:
		replyStr += std::to_string(context.getSize());
		break;
	case FTPReply::R227: {
		auto port = std::stoul(context.getPassiveSocket().getPort());
		auto portHi = std::to_string((port >> 8) & 0x00ff);
		auto portLo = std::to_string(port & 0x00ff);
		replyStr += context.getPasvAddr() + portHi + "," + portLo + ").";}
		break;
	case FTPReply::R257_PWD:
		replyStr += context.getCwd() + "\"";
		break;
	default:
		break;
	}

	std::cerr << ">     sent: " << replyStr << std::endl;

	return replyStr;
}

FTPContext& FTPClient::getContext() {
	return context;
}
