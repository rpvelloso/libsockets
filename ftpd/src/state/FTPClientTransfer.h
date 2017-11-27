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

#ifndef STATE_FTPCLIENTTRANSFER_H_
#define STATE_FTPCLIENTTRANSFER_H_

#include "state/FTPClientState.h"
#include <functional>

class FTPClientTransfer: public FTPClientState {
public:
	FTPClientTransfer(FTPClientInfo &ctx, std::ostream &outp, std::function<socks::ClientSocket()> getDataSocket);
	virtual ~FTPClientTransfer();
	StateType getState() override;

	FTPReply LIST(const std::string &path) override;
	FTPReply RETR(const std::string &filename) override;
	FTPReply STOR(const std::string &filename) override;
	FTPReply APPE(const std::string &filename) override;
	FTPReply REST(const std::string &pos) override;
	FTPReply PWD() override;
private:
	std::ostream &outp;
	static const size_t bufSize = 4*1024;
	void receiveFile(socks::ClientSocket& source, std::fstream &dest);
	void sendFile(std::fstream &source, socks::ClientSocket& dest);
	std::function<socks::ClientSocket()> getDataSocket;
};

#endif /* STATE_FTPCLIENTTRANSFER_H_ */
