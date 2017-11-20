/*
 * FTPClient.cpp
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#include "FTPClient.h"
#include "state/FTPClientConnected.h"
#include "state/FTPClientLoggedIn.h"
#include "state/FTPClientRename.h"
#include "state/FTPClientActive.h"

#include <sstream>
#include <algorithm>

FTPClient::FTPClient() : state(new FTPClientConnected(context)) {
}

FTPClient::~FTPClient() {
}

std::string FTPClient::processCmd(const std::string& cmdline) {
	std::stringstream ss(cmdline);
	std::string command;
	std::string param;

	ss >> command;
	std::getline(ss, param);
	param = param.substr(param.find_first_not_of(' '));
	if (param.back() == '\r')
		param.pop_back();

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
			try {
				state.reset(new FTPClientActive(context));
			} catch (std::exception &e) {
				state.reset(new FTPClientLoggedIn(context));
				reply = FTPReply::R425;
			}
		}
	} /*else if (command == "PASV")
		reply = state->PASV();*/
	else if (
			command == "LIST" ||
			command == "NLST") {
		reply = state->LIST(param, 1);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "RETR") {
		reply = state->RETR(param);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "STOR") {
		reply = state->STOR(param);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "APPE") {
		reply = state->APPE(param);
		state.reset(new FTPClientLoggedIn(context));
	} else if (command == "REST")
		reply = state->REST(param);
	else
		reply = FTPReply::R500;

	return buildReplyString(reply);
}

std::string FTPClient::buildReplyString(FTPReply reply) {
	std::string replyStr = FTPReplyString[reply];

	switch (reply) {
	case FTPReply::R200_TYPE:
		replyStr += context.getType() + "\n";
		break;
	case FTPReply::R213:
		replyStr += std::to_string(context.getSize()) + "\n";
		break;
	case FTPReply::R226:
		replyStr = FTPReplyString[FTPReply::R150] + replyStr;
		break;
	case FTPReply::R227: {
		auto port = std::stoul(context.getPassiveSocket().getPort());
		auto portHi = std::to_string((port >> 8) && 0x00ff);
		auto portLo = std::to_string(port && 0x00ff);
		replyStr += "127,0,0,1," + portHi + "," + portLo + "\n";}
		break;
	case FTPReply::R257_PWD:
		replyStr += context.getCwd() + "\"\n";
		break;
	default:
		break;
	}

	return replyStr;
}
