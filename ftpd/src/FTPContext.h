/*
 * FTPContext.h
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#ifndef FTPCONTEXT_H_
#define FTPCONTEXT_H_

#include "libsockets.h"

#include <string>
#include <fstream>

class FTPContext {
public:
	FTPContext();
	~FTPContext();
	const std::string& getUsername() const;
	void setUsername(const std::string& username = "");
	const std::string& getCwd() const;
	void setCwd(const std::string& cwd = "/");
	const std::string& getAddress() const;
	void setAddress(const std::string& address = "");
	const std::string& getPort() const;
	void setPort(const std::string& port = "");
	const std::string& getType() const;
	void setType(const std::string& type = "I");
	size_t getSize() const;
	void setSize(size_t size = 0);
	const std::string& getRenameFrom() const;
	void setRenameFrom(const std::string& renameFrom = "");
	socks::ServerSocket& getPassiveSocket() const;
	void setPassiveSocket(std::unique_ptr<socks::ServerSocket> serverSocket);
	std::fstream::pos_type getRestartPos() const;
	void setRestartPos(std::fstream::pos_type restartPos = 0);

private:
	std::string username = "";
	std::string cwd = "/";
	std::string address = "";
	std::string port = "";
	std::string type = "I";
	std::string renameFrom = "";
	size_t size = 0;
	std::fstream::pos_type restartPos = 0;

	std::unique_ptr<socks::ServerSocket> passiveSocket;
};

#endif /* FTPCONTEXT_H_ */
