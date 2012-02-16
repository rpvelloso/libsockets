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
#ifndef TFTPTHREAD_H_
#define TFTPTHREAD_H_

#include <libsockets/libsockets.h>
#include "tftplog.h"
#include "tftpclientsocket.h"
#include "tftpserver.h"

#define RECV_BUFLEN 1024

class tFTPServer;
class tFTPClientSocket;
class tFTPLog;

class tFTPThread : public tThread {
public:
	tFTPThread(int , tFTPServer *, tFTPClientSocket *);
	~tFTPThread();
    void Execute();
    void OnStart();
    void OnStop();
protected:
    tFTPClientSocket *socket;
    char buffer[RECV_BUFLEN];
    tFTPServer *owner;
    tFTPLog *log;
};

#endif
