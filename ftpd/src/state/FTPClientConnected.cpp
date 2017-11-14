/*
 * FTPClientConnected.cpp
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#include "FTPClientConnected.h"

FTPClientConnected::FTPClientConnected(FTPContext &ctx) : FTPClientState(ctx) {
}

FTPClientConnected::~FTPClientConnected() {
}

FTPReply FTPClientConnected::USER(const std::string &username) {
	if (validUserName(username)) {
		context.setUsername(username);
		return FTPReply::R331;
	} else
		return FTPReply::R501;
}

FTPReply FTPClientConnected::PASS(const std::string &password) {
	if (!context.getUsername().empty()) {
		// if authenticate OK (not implemented)
		return FTPReply::R230;
		// else
		// return FTPReply::R530;
	} else
		return FTPReply::R503;
}

bool FTPClientConnected::validUserName(const std::string& username) {
	return !username.empty();
}
