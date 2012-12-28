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
#include <cstdio>
#include <cstring>
#include <sstream>
#ifdef WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#endif
#include "AbstractSocket.h"

AbstractSocket::AbstractSocket() : Object() {
    socketFd = socket(PF_INET,SOCK_STREAM,0/*IPPROTO_TCP*/);
    socketStatus = SOCKET_CLOSED;
    hostname = "";
    nonBlocking = false;
    linger = -1;
}

AbstractSocket::~AbstractSocket() {
}

#define GETHOSTBYNAME_BUFSIZE 4096

bool AbstractSocket::resolveHost(string host) {
#ifdef WIN32
    struct hostent *hp;

    memset((void *)&socketAddress,0,sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    if ((hp=gethostbyname(host.c_str()))==NULL) return false;
    memcpy((void *)&socketAddress.sin_addr,(void *)hp->h_addr,hp->h_length);
#else
    struct hostent h, *hp;
    int h_errno;
    char hbuf[GETHOSTBYNAME_BUFSIZE];

    memset((void *)&socketAddress,0,sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    if (gethostbyname_r(host.c_str(),&h,hbuf,GETHOSTBYNAME_BUFSIZE,&hp,&h_errno)) return false;
    memcpy((void *)&(socketAddress.sin_addr.s_addr),(void *)h.h_addr,h.h_length);
#endif
    return true;
}

string AbstractSocket::getHostname() {
    if (hostname=="") {
#ifdef WIN32
       struct hostent *hp;

       hp=gethostbyaddr((char *)&socketAddress.sin_addr,sizeof(socketAddress.sin_addr),AF_INET);
       if (!hp) return NULL;
       hostname = hp->h_name;
#else
       struct hostent h, *hp;
       int h_errno;
       char hbuf[GETHOSTBYNAME_BUFSIZE];

       if (!gethostbyaddr_r((void *)&socketAddress.sin_addr,sizeof(socketAddress.sin_addr),AF_INET,&h,hbuf,GETHOSTBYNAME_BUFSIZE,&hp,&h_errno)) {
           hostname = h.h_name;
       }
#endif
    }
    return hostname;
}

unsigned short AbstractSocket::getPort() {
	return ntohs(socketAddress.sin_port);
}

string AbstractSocket::getIPAddress() {
	stringstream ret;
	int ip[4];

	ip[0] = ((char *)(&socketAddress.sin_addr))[0];
	ip[1] = ((char *)(&socketAddress.sin_addr))[1];
	ip[2] = ((char *)(&socketAddress.sin_addr))[2];
	ip[3] = ((char *)(&socketAddress.sin_addr))[3];
	ret<<ip[0]<<"."<<ip[1]<<"."<<ip[2]<<"."<<ip[3];

	return ret.str();
}

bool AbstractSocket::setLinger(int onoff, int ll) {
	struct linger l;
	int r;

	l.l_onoff = onoff;
	l.l_linger = ll;

	if ((r=setsockopt(socketFd,SOL_SOCKET,SO_LINGER,(char *)&l,sizeof(l))) != -1) onoff?linger = ll:linger = -1;
	return r==0;
}

int AbstractSocket::getLinger() {
	return linger;
}

#ifdef WIN32

bool AbstractSocket::setNonBlocking(bool nb) {
	unsigned long int socketFlags;
	int r;

	socketFlags = nb?1:0;
	if ((r=ioctlsocket(socketFd,FIONBIO,&socketFlags)) != -1) nonBlocking = nb;
	return r==0;
}

#else

bool AbstractSocket::setNonBlocking(bool nb) {
	int socketFlags,r;

	socketFlags=fcntl(socketFd,F_GETFL,0);
	if (nb) socketFlags |= O_NONBLOCK;
	else socketFlags &= ~O_NONBLOCK;

	if ((r=fcntl(socketFd,F_SETFL,socketFlags)) != -1) nonBlocking = nb;
	return r==0;
}

#endif

bool AbstractSocket::getNonBlocking() {
	return nonBlocking;
}

int AbstractSocket::getSocketFd() {
	return socketFd;
}

SocketStatus AbstractSocket::getSocketStatus() {
	return socketStatus;
}

