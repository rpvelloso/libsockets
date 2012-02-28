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
#ifndef WIN32
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/sendfile.h>
#endif
#include <string.h>
#include "tclientsocket.h"

tClientSocket::tClientSocket() : tSocket() {
	bytesIn = bytesOut = 0;
}

tClientSocket::tClientSocket(int fd, sockaddr_in *sin) {
    socketFd = fd;
    memcpy((void *)&socketAddr,(void *)sin,sizeof(sockaddr_in));
    socketStatus = tSocketOpened;
    hostname[0]=0;
	bytesIn = bytesOut = 0;
}

void tClientSocket::Close() {
    if (socketStatus != tSocketClosed) {
        onDisconnect();
#ifdef WIN32
        shutdown(socketFd,SD_BOTH);
#else
        shutdown(socketFd,SHUT_RDWR);
#endif
        close(socketFd);
        socketStatus = tSocketClosed;
    }
}

tClientSocket::~tClientSocket() {
}

int tClientSocket::Open(const char *addr, unsigned short port) {
    if (socketStatus == tSocketClosed) {
       if (resolveHost(addr)) return -1;
       socketAddr.sin_port = htons(port);
       if (connect(socketFd,(sockaddr *)&socketAddr,sizeof(socketAddr))==0) {
          socketStatus = tSocketOpened;
          onConnect();
          return 0;
       }
    }
    return -1;
}

int tClientSocket::Send(void *buf, size_t size) {
	ssize_t r;

    if (socketStatus == tSocketOpened) {
    	onSend(buf,&size);
#ifdef WIN32
        r = send(socketFd,(char *)buf,size,0);
#else
        r = send(socketFd,buf,size,0);
#endif
        if (r>0) bytesOut += r;
        return r;
    }
    return -1;
}

int tClientSocket::Send(string buf) {
	ssize_t r;

    if (socketStatus == tSocketOpened) {
    	size_t len=-1;

   		onSend(&buf,&len);
   		r = send(socketFd,buf.c_str(),buf.size(),0);
   		if (r>0) bytesOut += r;
   		return r;
    }
    return -1;
}


int tClientSocket::receive(void *buf, size_t size) {
    int r;

    if (socketStatus == tSocketOpened) {
#ifdef WIN32
        r = recv(socketFd,(char *)buf,size,0);
#else
        r = recv(socketFd,buf,size,0);
#endif
       if (r > 0) {
    	   bytesIn += r;
    	   onReceive(buf,r);
       }
       if (r == 0) {
    	   socketStatus = tSocketClosed;
    	   onDisconnect();
       }
       return r;
    }
    return -1;
}

#define SFBUF_SIZE 4096
ssize_t tClientSocket::sendFile(const char *path, off_t *offset, ssize_t count) {
	ssize_t r=0;
	int fd;

#ifdef WIN32
	if ((fd = open(path,O_RDONLY|O_BINARY)) > 0) {
		char sfbuf[SFBUF_SIZE];
		int len;

		lseek(fd,*offset,SEEK_SET);
		while ((r < count) && (len=read(fd,sfbuf,SFBUF_SIZE))>0) {
			if (Send(sfbuf,len)<=0) {
				perror("Send()");
				break;
			}
			r+=len;
		}
#else
	if ((fd = open(path,O_RDONLY)) > 0) {
		r = sendfile(socketFd,fd,offset,count);
		if (r>0) bytesOut += r;
#endif
		close(fd);
		return r;
	}
	return fd;
}
