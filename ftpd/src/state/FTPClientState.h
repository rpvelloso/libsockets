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

#include <unordered_map>
#include <string>

#include "FTPContext.h"

enum class FTPReply {
	RNULL, R150, R200, R200_MODE, R200_TYPE, R200_STRU, R211,
	R213, R215, R220, R221, R226, R227, R230, R250,
	R257, R257_PWD, R331, R350, R350_RNFR, R421, R425,
	R426, R500, R501, R503, R503_RNTO, R504, R530,
	R550_MKD, R550_SIZE1, R550_SIZE2, R550_CWD, R550_RETR,
	R550_STOR, R550_DELE, R550_RNTO
};

extern std::unordered_map<FTPReply, std::string> FTPReplyString;

class FTPClientState {
public:
	FTPClientState(FTPContext &ctx);
	virtual ~FTPClientState();
	virtual FTPReply NOOP();

	virtual FTPReply QUIT();
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

	virtual FTPReply LIST(const std::string &, int);
	virtual FTPReply RETR(const std::string &);
	virtual FTPReply STOR(const std::string &);
	virtual FTPReply APPE(const std::string &);
	virtual FTPReply REST(const std::string &);

protected:
	FTPContext &context;
};

#endif /* FTPCLIENTSTATE_H_ */
