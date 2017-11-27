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

#ifndef FTPCLIENTINFO_H_
#define FTPCLIENTINFO_H_

#include "libsockets.h"

#include <string>
#include <fstream>

enum class FTPReply {
	RNULL, R150, R200, R200_MODE, R200_TYPE, R200_STRU, R211,
	R213, R215, R220, R221, R226, R227, R230, R250,
	R257, R257_PWD, R331, R350, R350_RNFR, R421, R425,
	R426, R500, R501, R503, R503_RNTO, R504, R530,
	R550_MKD, R550_SIZE1, R550_SIZE2, R550_CWD, R550_RETR,
	R550_STOR, R550_DELE, R550_RNTO
};

extern std::unordered_map<FTPReply, std::string> FTPReplyString;

class FTPClientInfo;
using AuthenticationFunction = std::function<bool(const std::string &username, const std::string &password, FTPClientInfo &clientInfo)>;

class FTPClientInfo {
public:
	FTPClientInfo();
	std::string buildReplyString(FTPReply reply);

	const std::string& getUsername() const;
	void setUsername(const std::string& username);
	const std::string& getCwd() const;
	void setCwd(const std::string& cwd);
	const std::string& getAddress() const;
	void setAddress(const std::string& address);
	const std::string& getPort() const;
	void setPort(const std::string& port);
	const std::string& getType() const;
	void setType(const std::string& type);
	size_t getSize() const;
	void setSize(size_t size = 0);
	const std::string& getRenameFrom() const;
	void setRenameFrom(const std::string& renameFrom);
	socks::ServerSocket& getPassiveSocket() const;
	void setPassiveSocket(std::unique_ptr<socks::ServerSocket> serverSocket);
	std::fstream::pos_type getRestartPos() const;
	void setRestartPos(std::fstream::pos_type restartPos = 0);
	const std::string& getPasvAddr() const;
	void setPasvAddr(socks::SocketAddress &addr);
	const std::string& getPeerAddr() const;
	void setPeerAddr(socks::SocketAddress &addr);
	static AuthenticationFunction authenticate;
private:
	std::string username;
	std::string cwd = "/";
	std::string address;
	std::string port;
	std::string type = "I";
	std::string renameFrom;
	size_t size = 0;
	std::fstream::pos_type restartPos = 0;
	std::string pasvAddr;
	std::string peerAddr;
	std::unique_ptr<socks::ServerSocket> passiveSocket;

	std::string socketAddr2FTPAddr(socks::SocketAddress &addr);
};

#endif /* FTPCLIENTINFO_H_ */
