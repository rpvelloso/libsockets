/*
 * FTPClientActive.h
 *
 *  Created on: 14 de nov de 2017
 *      Author: rvelloso
 */

#ifndef STATE_FTPCLIENTACTIVE_H_
#define STATE_FTPCLIENTACTIVE_H_

#include <state/FTPClientState.h>

class FTPClientActive: public FTPClientState {
public:
	FTPClientActive(FTPContext &ctx);
	virtual ~FTPClientActive();

	FTPReply LIST(const std::string &path, int type) override;
	FTPReply RETR(const std::string &filename) override;
	FTPReply STOR(const std::string &filename) override;
	FTPReply APPE(const std::string &filename) override;
	FTPReply REST(const std::string &pos) override;
private:
	void receiveFile(socks::ClientSocket& source, std::fstream &dest);
	socks::SocketStream dataSocket;
};

#endif /* STATE_FTPCLIENTACTIVE_H_ */
