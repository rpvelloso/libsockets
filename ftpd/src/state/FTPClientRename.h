/*
 * FTPClientRename.h
 *
 *  Created on: 13 de nov de 2017
 *      Author: rvelloso
 */

#ifndef STATE_FTPCLIENTRENAME_H_
#define STATE_FTPCLIENTRENAME_H_

#include "filesystem/FileSystem.h"
#include "state/FTPClientState.h"

class FTPClientRename: public FTPClientState {
public:
	FTPClientRename(FTPContext& ctx);
	virtual ~FTPClientRename();

	FTPReply RNTO(const std::string &path) override;
};

#endif /* STATE_FTPCLIENTRENAME_H_ */
