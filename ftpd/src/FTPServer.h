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

#ifndef FTPSERVER_H_
#define FTPSERVER_H_

#include <functional>
#include <map>
#include <atomic>

#include "FTPClient.h"

class FTPServer {
public:
	FTPServer(bool ssl = false, bool verbose = false, const std::string & = "");
	void start();
	void registerSiteCommand(const std::string &command, SiteCallback callback);
	size_t getClientCount();
private:
	socks::Server server;
	bool verbose;
	std::string port;
	std::map<std::string, SiteCallback> siteCommands;
	std::atomic<size_t> clientCount;

	static std::string readline(std::istream &inp);
	void onConnect(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp);
	void onDisconnect(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp);
	void onReceive(socks::Context<FTPClient> &ctx, std::istream &inp, std::ostream &outp);
	socks::Server makeSSLFTPServer();
	socks::Server makeFTPServer();
};

#endif /* FTPSERVER_H_ */
