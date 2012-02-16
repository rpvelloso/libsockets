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
	bytes_in = bytes_out = 0;
}

tClientSocket::tClientSocket(int fd, sockaddr_in *sin) {
    socket_fd = fd;
    memcpy((void *)&socket_addr,(void *)sin,sizeof(sockaddr_in));
    socket_status = tSocketOpened;
    hostname[0]=0;
	bytes_in = bytes_out = 0;
}

void tClientSocket::Close() {
    if (socket_status != tSocketClosed) {
        OnDisconnect();
#ifdef WIN32
        shutdown(socket_fd,SD_BOTH);
        closesocket(socket_fd);
#else
        shutdown(socket_fd,SHUT_RDWR);
        close(socket_fd);
#endif
        socket_status = tSocketClosed;
    }
}

tClientSocket::~tClientSocket() {
}

int tClientSocket::Open(const char *addr, unsigned short port) {
    if (socket_status == tSocketClosed) {
       if (ResolveHost(addr)) return -1;
       socket_addr.sin_port = htons(port);
       if (connect(socket_fd,(sockaddr *)&socket_addr,sizeof(socket_addr))==0) {
          socket_status = tSocketOpened;
          OnConnect();
          return 0;
       }
    }
    return -1;
}

int tClientSocket::Send(void *buf, size_t size) {
	ssize_t r;

    if (socket_status == tSocketOpened) {
    	OnSend(buf,&size);
#ifdef WIN32
        r = send(socket_fd,(char *)buf,size,0);
#else
        r = send(socket_fd,buf,size,0);
#endif
        if (r>0) bytes_out += r;
        return r;
    }
    return -1;
}

int tClientSocket::Send(string buf) {
	ssize_t r;

    if (socket_status == tSocketOpened) {
    	size_t len=-1;

   		OnSend(&buf,&len);
   		r = send(socket_fd,buf.c_str(),buf.size(),0);
   		if (r>0) bytes_out += r;
   		return r;
    }
    return -1;
}


int tClientSocket::Receive(void *buf, size_t size) {
    int r;

    if (socket_status == tSocketOpened) {
#ifdef WIN32
        r = recv(socket_fd,(char *)buf,size,0);
#else
        r = recv(socket_fd,buf,size,0);
#endif
       if (r > 0) {
    	   bytes_in += r;
    	   OnReceive(buf,r);
       }
       if (r == 0) {
    	   socket_status = tSocketClosed;
    	   OnDisconnect();
       }
       return r;
    }
    return -1;
}

#define SFBUF_SIZE 4096
ssize_t tClientSocket::SendFile(const char *path, off_t *offset, ssize_t count) {
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
		r = sendfile(socket_fd,fd,offset,count);
		if (r>0) bytes_out += r;
#endif
		close(fd);
		return r;
	}
	return fd;
}
