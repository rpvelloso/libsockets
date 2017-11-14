/*
 * FTPClientLoggedIn.h
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#ifndef STATE_FTPCLIENTLOGGEDIN_H_
#define STATE_FTPCLIENTLOGGEDIN_H_

#include <state/FTPClientState.h>

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
