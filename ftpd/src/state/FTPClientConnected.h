/*
 * FTPClientConnected.h
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
 */

#ifndef FTPCLIENTCONNECTED_H_
#define FTPCLIENTCONNECTED_H_

#include "FTPClientState.h"

class FTPClientConnected: public FTPClientState {
public:
	FTPClientConnected(FTPContext &ctx);
	virtual ~FTPClientConnected();
	FTPReply USER(const std::string &) override;
	FTPReply PASS(const std::string &) override;
private:
	bool validUserName(const std::string &);
};

#endif /* FTPCLIENTCONNECTED_H_ */
