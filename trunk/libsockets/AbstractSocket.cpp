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
#include <netdb.h>
#include "AbstractSocket.h"

AbstractSocket::AbstractSocket() : Object() {
    socketFd = socket(PF_INET,SOCK_STREAM|SOCK_CLOEXEC,0/*IPPROTO_TCP*/);
    socketStatus = SOCKET_CLOSED;
    hostname = "";
    nonBlocking = false;
    linger = -1;
}

AbstractSocket::~AbstractSocket() {
}

bool AbstractSocket::resolveHost(string host) {
    struct addrinfo *res;

    memset((void *)&socketAddress,0,sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    if (getaddrinfo(host.c_str(),NULL,NULL,&res) != 0) return false;
    memcpy((void *)&socketAddress,(void *)res->ai_addr,sizeof(struct sockaddr_in));
    socketAddress.sin_addr.s_addr = ((sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(res);
    return true;
}

string AbstractSocket::getHostname() {
    if (hostname=="") {
       char hn[BUFSIZ];

       if (getnameinfo((struct sockaddr *)&socketAddress, sizeof(socketAddress),hn, BUFSIZ,NULL,0,0) == 0)
    	   hostname = hn;
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

bool AbstractSocket::setNonBlocking(int fd, bool nb) {
	int socketFlags;

	socketFlags=fcntl(fd,F_GETFL,0);
	if (nb) socketFlags |= O_NONBLOCK;
	else socketFlags &= ~O_NONBLOCK;

	return (fcntl(fd,F_SETFL,socketFlags) == 0);
}

bool AbstractSocket::setNonBlocking(bool nb) {
	bool r = setNonBlocking(socketFd,nb);

	if (r) nonBlocking = nb;
	return r;
}

bool AbstractSocket::getNonBlocking() {
	return nonBlocking;
}

int AbstractSocket::getSocketFd() {
	return socketFd;
}

SocketStatus AbstractSocket::getSocketStatus() {
	return socketStatus;
}

bool AbstractSocket::setCloseOnExec(int fd) {
	int f = fcntl(fd,F_GETFD);

	if (f!=-1) {
		f=fcntl(fd,F_SETFD,f|FD_CLOEXEC);
	}
	return f!=-1;
}
