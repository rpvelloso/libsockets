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
#include <fcntl.h>
#include <sstream>
#include "tsocket.h"

tSocket::tSocket() {
    socketFd = socket(PF_INET,SOCK_STREAM,0/*IPPROTO_TCP*/);
    socketStatus = tSocketClosed;  
    hostname[0] = 0;                 
	nonBlockingIO = 0;
}

tSocket::~tSocket() {
}

tSocketStatus tSocket::getStatus() {
    return socketStatus;
}    

#define GETHOSTBYNAME_BUFSIZE 4096

int tSocket::resolveHost(const char *host) {
#ifdef WIN32
    struct hostent *hp;

    memset((void *)&socketAddr,0,sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    if ((hp=gethostbyname(host))==NULL) return -1;
    memcpy((void *)&socketAddr.sin_addr,(void *)hp->h_addr,hp->h_length);
#else
    struct hostent h, *hp;
    int h_errno;
    char hbuf[GETHOSTBYNAME_BUFSIZE];

    memset((void *)&socketAddr,0,sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    if (gethostbyname_r(host,&h,hbuf,GETHOSTBYNAME_BUFSIZE,&hp,&h_errno)) return -1;
    memcpy((void *)&(socketAddr.sin_addr.s_addr),(void *)h.h_addr,h.h_length);
#endif
    return 0;
}

char *tSocket::getHostname() {

    if (!hostname[0]) {
#ifdef WIN32
       struct hostent *hp;

       hp=gethostbyaddr((char *)&socketAddr.sin_addr,sizeof(socketAddr.sin_addr),AF_INET);
       if (!hp) return NULL;
       strncpy(hostname,hp->h_name,MAX_HOSTNAME-1);
       hostname[MAX_HOSTNAME-1]=0;
#else
       struct hostent h, *hp;
       int h_errno;
       char hbuf[GETHOSTBYNAME_BUFSIZE];

       if (!gethostbyaddr_r((void *)&socketAddr.sin_addr,sizeof(socketAddr.sin_addr),AF_INET,&h,hbuf,GETHOSTBYNAME_BUFSIZE,&hp,&h_errno)) {
           strncpy(hostname,h.h_name,MAX_HOSTNAME-1);
           hostname[MAX_HOSTNAME-1]=0;
       }
#endif
    }
    return hostname;
}

string tSocket::getIP() {
	stringstream ret;
	int ip[4];

	ip[0] = ((char *)(&socketAddr.sin_addr))[0];
	ip[1] = ((char *)(&socketAddr.sin_addr))[1];
	ip[2] = ((char *)(&socketAddr.sin_addr))[2];
	ip[3] = ((char *)(&socketAddr.sin_addr))[3];
	ret<<ip[0]<<"."<<ip[1]<<"."<<ip[2]<<"."<<ip[3];

	return ret.str();
}

unsigned short tSocket::getPort() {
	return ntohs(socketAddr.sin_port);
}

int tSocket::setLinger(int onoff, int ll) {
	struct linger l;

	l.l_onoff = onoff;
	l.l_linger = ll;

	return setsockopt(socketFd,SOL_SOCKET,SO_LINGER,(char *)&l,sizeof(l));
}

#ifdef WIN32

int tSocket::toggleNonBlockingIO() {
	unsigned long int socketFlags;
	int r;

	socketFlags = !nonBlockingIO;
	if (((r=ioctlsocket(socketFd,FIONBIO,&socketFlags)) != -1) nonBlockingIO = !nonBlockingIO;
	return r;
}

#else

int tSocket::toggleNonBlockingIO() {
	int socketFlags,r;

	socketFlags=fcntl(socketFd,F_GETFL,0);
	if (!nonBlockingIO) socketFlags |= O_NONBLOCK;
	else socketFlags &= ~O_NONBLOCK;

	if ((r=fcntl(socketFd,F_SETFL,socketFlags)) != -1) nonBlockingIO = !nonBlockingIO;;
	return r;
}

#endif

int tSocket::getSocketFd() {
	return socketFd;
}

tBlockingIOState tSocket::getBlockingIOState() {
	return nonBlockingIO?tNonBlocking:tBlocking;
}

#ifdef WIN32
int WinSocketStartup() {
     WSADATA info;
     return WSAStartup(MAKEWORD(2,0), &info);
}
#endif
