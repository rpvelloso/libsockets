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
	static const size_t bufSize = 4*1024;
	void receiveFile(socks::ClientSocket& source, std::fstream &dest);
	void sendFile(std::fstream &source, socks::ClientSocket& dest);
	socks::ClientSocket dataSocket;
};

#endif /* STATE_FTPCLIENTACTIVE_H_ */
