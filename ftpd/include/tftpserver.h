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
#ifndef TFTPSERVER_H_
#define TFTPSERVER_H_

#include <string>
#include <list>
#include <libsockets/libsockets.h>
#include "tftplog.h"
#include "tftpserversocket.h"
#include "tftpthread.h"

class tFTPServerSocket;
class tFTPThread;

class tLogin : public tObject {
public:
	string username;
	string password;
	string homedir;
};

class tFTPServer : public tObject {
public:
	tFTPServer(int, string);
	~tFTPServer();
	void run(const char *, unsigned short);
	void stop();
	void removeThread(tFTPThread *);
	void addThread(tFTPThread *);
	tFTPLog *getLog();
	void setMaxClients(int);
	int getMaxClients();
	int getTotalClients();
	void setTotalClients(int);
	tLogin *authenticate(string, string);
protected:
	void loadLogins(string);
	list<tLogin *>users;
	tFTPServerSocket *serverSocket;
	list<tFTPThread *> threads;
	tMutex *threadsMutex;
	tFTPLog *log;
	int totalClients,maxClients;
	tMutex *totalClientsMutex;
};

#endif
