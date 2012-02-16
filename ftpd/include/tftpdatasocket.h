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
	void SetListType(int);
	void List(string, int);
	void Retrieve(string, off_t);
	void Store(string, off_t);
	tFTPClientSocket *GetControlConnection();
	void SetControlConnection(tFTPClientSocket *);
	void SetTransferComplete(int);
	void OnSend(void *, size_t *);
	void OnReceive(void *, size_t);
	void OnConnect();
	void OnDisconnect();
protected:
	int transfer_complete;
	tFTPClientSocket *control_connection;
};

#endif
