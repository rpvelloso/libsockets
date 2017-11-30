/*
 * FTPServer.h
 *
 *  Created on: 30 de nov de 2017
 *      Author: rvelloso
 */

#ifndef FTPSERVER_H_
#define FTPSERVER_H_

#include <functional>
#include <map>
#include <atomic>

#include "libsockets.h"
#include "FTPClient.h"
#include "FTPClientInfo.h"
#include "auth/Authentication.h"

class FTPServer {
public:
	FTPServer(bool ssl = false, const std::string & = "");
	void start();
	void registerSiteCommand(const std::string &command, SiteCallback callback);
	size_t getClientCount();
private:
	socks::Server server;
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
