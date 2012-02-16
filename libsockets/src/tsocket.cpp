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
#include <sstream>
#include "tsocket.h"

tSocket::tSocket() {
    socket_fd = socket(PF_INET,SOCK_STREAM,0/*IPPROTO_TCP*/);
    socket_status = tSocketClosed;  
    hostname[0] = 0;                 
}

tSocket::~tSocket() {
}

tSocketStatus tSocket::GetStatus() {
    return socket_status;
}    

#define GETHOSTBYNAME_BUFSIZE 4096

int tSocket::ResolveHost(const char *host) {
#ifdef WIN32
    struct hostent *hp;

    memset((void *)&socket_addr,0,sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    if ((hp=gethostbyname(host))==NULL) return -1;
    memcpy((void *)&socket_addr.sin_addr,(void *)hp->h_addr,hp->h_length);
#else
    struct hostent h, *hp;
    int h_errno;
    char hbuf[GETHOSTBYNAME_BUFSIZE];

    memset((void *)&socket_addr,0,sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    if (gethostbyname_r(host,&h,hbuf,GETHOSTBYNAME_BUFSIZE,&hp,&h_errno)) return -1;
    memcpy((void *)&(socket_addr.sin_addr.s_addr),(void *)h.h_addr,h.h_length);
#endif
    return 0;
}

char *tSocket::GetHostName() {

    if (!hostname[0]) {
#ifdef WIN32
       struct hostent *hp;

       hp=gethostbyaddr((char *)&socket_addr.sin_addr,sizeof(socket_addr.sin_addr),AF_INET);
       if (!hp) return NULL;
       strncpy(hostname,hp->h_name,MAX_HOSTNAME-1);
       hostname[MAX_HOSTNAME-1]=0;
#else
       struct hostent h, *hp;
       int h_errno;
       char hbuf[GETHOSTBYNAME_BUFSIZE];

       if (!gethostbyaddr_r((void *)&socket_addr.sin_addr,sizeof(socket_addr.sin_addr),AF_INET,&h,hbuf,GETHOSTBYNAME_BUFSIZE,&hp,&h_errno)) {
           strncpy(hostname,h.h_name,MAX_HOSTNAME-1);
           hostname[MAX_HOSTNAME-1]=0;
       }
#endif
    }
    return hostname;
}

string tSocket::GetIP() {
	stringstream ret;
	int ip[4];

	ip[0] = ((char *)(&socket_addr.sin_addr))[0];
	ip[1] = ((char *)(&socket_addr.sin_addr))[1];
	ip[2] = ((char *)(&socket_addr.sin_addr))[2];
	ip[3] = ((char *)(&socket_addr.sin_addr))[3];
	ret<<ip[0]<<"."<<ip[1]<<"."<<ip[2]<<"."<<ip[3];

	return ret.str();
}

unsigned short tSocket::GetPort() {
	return ntohs(socket_addr.sin_port);
}

int tSocket::SetLinger(int onoff, int ll) {
	struct linger l;

	l.l_onoff = onoff;
	l.l_linger = ll;

	return setsockopt(socket_fd,SOL_SOCKET,SO_LINGER,(char *)&l,sizeof(l));
}

#ifdef WIN32
int WinSocketStartup() {
     WSADATA info;
     return WSAStartup(MAKEWORD(2,0), &info);
}
#endif
