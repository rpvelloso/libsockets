/*
    Copyright 2011 Roberto Panerai Velloso.

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
#ifndef TFTPDATASOCKET_H_
#define TFTPDATASOCKET_H_

#include <string>
#include <libsockets/libsockets.h>
#include "tftpclientsocket.h"

#define STOR_PERM 0640

#define STOR_BUFLEN 4096
#define ERRSTR_LEN 20

#ifdef WIN32 // under windows, this functions are thread-safe
	#define readdir_r(i,j,k) *k=readdir(i)
#endif

class tFTPClientSocket;

class tFTPDataSocket : public tClientSocket {
public:
	tFTPDataSocket();
	tFTPDataSocket(int, sockaddr_in *);
	~tFTPDataSocket();
	void setListType(int);
	void list(string, int);
	void retrieve(string, off_t);
	void store(string, off_t);
	tFTPClientSocket *getControlConnection();
	void setControlConnection(tFTPClientSocket *);
	void setTransferComplete(int);
	void onSend(void *, size_t *);
	void onReceive(void *, size_t);
	void onConnect();
	void onDisconnect();
protected:
	int transferComplete;
	tFTPClientSocket *controlConnection;
};

#endif
