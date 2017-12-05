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

#include <sstream>
#include <algorithm>

FTPClient::FTPClient() : state(new FTPClientConnected(clientInfo)) {
}

FTPClient::~FTPClient() {
}

FTPReply FTPClient::processCmd(const std::string& cmdline, std::ostream &outp) {
	std::stringstream ss(cmdline);
	std::string command;
	std::string param;

	if (clientInfo.isVerbose())
		std::cerr << "< received: " << cmdline << std::endl;

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
	else if (command == "REIN") {
		reply = state->REIN();
		if (reply == FTPReply::R200)
			state.reset(new FTPClientConnected(clientInfo));
	} else if (command == "USER")
		reply = state->USER(param);
	else if (command == "PASS") {
		reply = state->PASS(param);
		if (reply == FTPReply::R230)
			state.reset(new FTPClientLoggedIn(clientInfo));
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
			state.reset(new FTPClientRename(clientInfo));
	} else if (command == "RNTO") {
		reply = state->RNTO(param);
		if (reply == FTPReply::R250)
			state.reset(new FTPClientLoggedIn(clientInfo));
	} else if (command == "PORT") {
		reply = state->PORT(param);
		if (reply == FTPReply::R200) {
			state.reset(new FTPClientTransfer(clientInfo, outp, [this]() {
				socks::ClientSocket dataSocket;
				if (dataSocket.connectTo(this->getClientInfo().getAddress(), clientInfo.getPort()) != 0)
					throw std::runtime_error("Can't open data connection.");
				return dataSocket;
			}));
		}
	} else if (command == "PASV") {
		reply = state->PASV();
		if (reply == FTPReply::R227) {
			outp << clientInfo.buildReplyString(reply) << std::endl;
			reply = FTPReply::RNULL;
			state.reset(new FTPClientTransfer(clientInfo, outp, [this]() {
				auto dataSocket = this->getClientInfo().getPassiveSocket().acceptConnection();
				if (this->getClientInfo().getPeerAddr() != dataSocket.getRemoteAddress().getHostname())
					throw std::runtime_error("Invalid peer address.");
				clientInfo.getPassiveSocket().disconnect();
				return dataSocket;
			}));
		}
	} else if (
			command == "LIST" ||
			command == "NLST") {
		reply = state->LIST(param);
		if (state->getState() == StateType::Transfer)
			state.reset(new FTPClientLoggedIn(clientInfo));
	} else if (command == "RETR") {
		reply = state->RETR(param);
		if (state->getState() == StateType::Transfer)
			state.reset(new FTPClientLoggedIn(clientInfo));
	} else if (command == "STOR") {
		reply = state->STOR(param);
		if (state->getState() == StateType::Transfer)
			state.reset(new FTPClientLoggedIn(clientInfo));
	} else if (command == "APPE") {
		reply = state->APPE(param);
		if (state->getState() == StateType::Transfer)
			state.reset(new FTPClientLoggedIn(clientInfo));
	} else if (command == "REST")
		reply = state->REST(param);
	else if (command == "SITE")
		reply = state->SITE(param);
	else
		reply = FTPReply::R500;

	return reply;
}

FTPClientInfo& FTPClient::getClientInfo() {
	return clientInfo;
}

