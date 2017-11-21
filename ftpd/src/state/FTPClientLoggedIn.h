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

#ifndef STATE_FTPCLIENTLOGGEDIN_H_
#define STATE_FTPCLIENTLOGGEDIN_H_

#include "state/FTPClientState.h"

class FTPClientLoggedIn: public FTPClientState {
public:
	FTPClientLoggedIn(FTPContext &ctx);
	virtual ~FTPClientLoggedIn();

	FTPReply MODE(const std::string &) override;
	FTPReply STRU(const std::string &) override;
	FTPReply TYPE(const std::string &) override;
	FTPReply CWD(const std::string &) override;
	FTPReply MKD(const std::string &) override;
	FTPReply SIZE(const std::string &) override;
	FTPReply HELP() override;
	FTPReply SYST() override;
	FTPReply PWD() override;
	FTPReply DELE(const std::string &) override;
	FTPReply RNFR(const std::string &) override;

	FTPReply PORT(const std::string &) override;
	FTPReply PASV() override;
};

#endif /* STATE_FTPCLIENTLOGGEDIN_H_ */
