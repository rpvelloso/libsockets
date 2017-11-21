/*
 * FTPClient.h
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#ifndef FTPCLIENT_H_
#define FTPCLIENT_H_

#include <memory>

#include "FTPContext.h"
#include "state/FTPClientState.h"

class FTPClient {
public:
	FTPClient();
	~FTPClient();
	std::string processCmd(const std::string &cmdline, std::ostream &outp);
	std::string buildReplyString(FTPReply reply);
private:
	FTPContext context;
	std::unique_ptr<FTPClientState> state;
};

#endif /* FTPCLIENT_H_ */
