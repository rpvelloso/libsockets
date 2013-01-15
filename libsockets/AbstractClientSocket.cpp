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

#include <cerrno>
#include <cstring>
#include "AbstractClientSocket.h"

AbstractClientSocket::AbstractClientSocket() : AbstractSocket() {
	bytesIn = bytesOut = 0;
}

AbstractClientSocket::AbstractClientSocket(int fd, sockaddr_in* sin) {
    socketFd = fd;
    memcpy((void *)&socketAddress,(void *)sin,sizeof(sockaddr_in));
    socketStatus = SOCKET_OPENED;
    hostname="";
	bytesIn = bytesOut = 0;
}

AbstractClientSocket::~AbstractClientSocket() {
}

bool AbstractClientSocket::openSocket(string addr, unsigned short port) {
    if (socketStatus == SOCKET_CLOSED) {
       if (!resolveHost(addr)) return false;
       socketAddress.sin_port = htons(port);
       if (connect(socketFd,(sockaddr *)&socketAddress,sizeof(socketAddress))==0) {
          socketStatus = SOCKET_OPENED;
          onConnect();
          return true;
       }
    }
    return false;
}

void AbstractClientSocket::closeSocket() {
	if (socketStatus != SOCKET_CLOSED) {
		onDisconnect();
        shutdown(socketFd,SHUT_RDWR);
        close(socketFd);
        socketStatus = SOCKET_CLOSED;
    }
}

int AbstractClientSocket::sendData(void *buf, size_t size) {
	ssize_t r=0;

    if (socketStatus == SOCKET_OPENED) {
    	beforeSend(buf,size);
    	if (size) {
			r = send(socketFd,buf,size,0);
			if (r>0) {
				bytesOut += r;
				onSend(buf,r);
			} else {
			   if (!((r<0) && (
					(errno == EAGAIN) ||
					(errno == EBUSY) ||
					(errno == EINTR)))) {
				   socketStatus = SOCKET_CLOSED;
				   onDisconnect();
			   }
			}
    	}
        return r;
    }
    return -1;
}

int AbstractClientSocket::sendData(string buf) {
	ssize_t r = 0;
	size_t size = 0;

    if (socketStatus == SOCKET_OPENED) {
    	beforeSend(&buf,size);
    	if (buf.size() > 0) {
			r = send(socketFd,buf.c_str(),buf.size(),0);
			if (r>0) {
				string sent = buf.substr(0,r);
				bytesOut += r;
				onSend(&sent,0);
			} else {
			   if (!((r<0) && (
					(errno == EAGAIN) ||
					(errno == EBUSY) ||
					(errno == EINTR)))) {
				   socketStatus = SOCKET_CLOSED;
				   onDisconnect();
			   }
			}
    	}
   		return r;
    }
    return -1;
}

int AbstractClientSocket::receiveData(void *buf, size_t size) {
    int r;

    if (socketStatus == SOCKET_OPENED) {
        r = recv(socketFd,buf,size,0);
       if (r > 0) {
    	   bytesIn += r;
    	   onReceive(buf,r);
       } else {
    	   if (!((r<0) && (
    			(errno == EAGAIN) ||
    			(errno == EBUSY) ||
    			(errno == EINTR)))) {
    		   socketStatus = SOCKET_CLOSED;
    		   onDisconnect();
    	   }
       }
       return r;
    }
    return -1;
}
