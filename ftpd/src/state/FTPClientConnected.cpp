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

#include "FTPClientConnected.h"

FTPClientConnected::FTPClientConnected(FTPClientInfo &ctx) : FTPClientState(ctx) {
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
		if (context.authenticate(context.getUsername(), password, context))
			return FTPReply::R230;
		else
			return FTPReply::R530;
	} else
		return FTPReply::R503;
}

bool FTPClientConnected::validUserName(const std::string& username) {
	return !username.empty();
}
