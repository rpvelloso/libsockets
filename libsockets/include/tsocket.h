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
#ifndef TSOCKET_H_
#define TSOCKET_H_

#ifdef WIN32
    #include <winsock2.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif
#include <string.h>
#ifdef DEVEL_ENV
	#include "tobject.h"
#else
	#include <libsockets/tobject.h>
#endif

using namespace std;

enum tSocketStatus {
     tSocketOpened,
     tSocketClosed,
     tSocketListening
};

enum tBlockingIOState {
	tBlocking=0,
	tNonBlocking
};

#define MAX_HOSTNAME 256

class tSocket : public tObject {
public:
       tSocket();
       virtual ~tSocket();
       tSocketStatus getStatus();
       virtual int Open(const char *, unsigned short) = 0;
       virtual void Close() = 0;
       int resolveHost(const char *);
       char *getHostname();
       unsigned short getPort();
       string getIP();
       int setLinger(int,int);
       int toggleNonBlockingIO();
       tBlockingIOState getBlockingIOState();
       int getSocketFd();
protected:
        int socketFd;
        tSocketStatus socketStatus;
        sockaddr_in socketAddr;        
        char hostname[MAX_HOSTNAME];
        int nonBlockingIO;
};

#ifdef WIN32
int WinSocketStartup();
#endif

#endif /* TSOCKET_H_ */
