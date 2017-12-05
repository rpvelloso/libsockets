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

#ifndef FTPCLIENTSTATE_H_
#define FTPCLIENTSTATE_H_

#include "FTPClientInfo.h"
#include <unordered_map>
#include <string>

enum class StateType {
	Connected,
	LoggedIn,
	Rename,
	Transfer
};

class FTPClientState {
public:
	FTPClientState(FTPClientInfo &ctx);
	virtual ~FTPClientState();
	virtual StateType getState() = 0;

	virtual FTPReply NOOP();

	virtual FTPReply QUIT();
	virtual FTPReply REIN();
	virtual FTPReply USER(const std::string &);
	virtual FTPReply PASS(const std::string &);

	virtual FTPReply MODE(const std::string &);
	virtual FTPReply STRU(const std::string &);
	virtual FTPReply TYPE(const std::string &);
	virtual FTPReply CWD(const std::string &);
	virtual FTPReply MKD(const std::string &);
	virtual FTPReply SIZE(const std::string &);
	virtual FTPReply HELP();
	virtual FTPReply SYST();
	virtual FTPReply PWD();
	virtual FTPReply DELE(const std::string &);
	virtual FTPReply RNFR(const std::string &);
	virtual FTPReply RNTO(const std::string &);

	virtual FTPReply PORT(const std::string &);
	virtual FTPReply PASV();

	virtual FTPReply LIST(const std::string &);
	virtual FTPReply RETR(const std::string &);
	virtual FTPReply STOR(const std::string &);
	virtual FTPReply APPE(const std::string &);
	virtual FTPReply REST(const std::string &);

	virtual FTPReply SITE(const std::string &);

protected:
	FTPClientInfo &clientInfo;
};

#endif /* FTPCLIENTSTATE_H_ */
