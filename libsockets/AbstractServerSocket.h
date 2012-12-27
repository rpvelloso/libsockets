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

#ifndef ABSTRACTSERVERSOCKET_H_
#define ABSTRACTSERVERSOCKET_H_

#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include "AbstractSocket.h"

#define SERVER_BACKLOG 5

template <class C>
class AbstractServerSocket: public AbstractSocket {
public:
	AbstractServerSocket() : AbstractSocket() {
 	   int reuse=1;

 	   if (setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(int))<0) perror("setsockopt()");
	};

	virtual ~AbstractServerSocket() {};

    virtual bool openSocket(string addr, unsigned short port) {
 #ifdef WIN32
    	int size = sizeof(socketAddress);
 #else
    	socklen_t size = sizeof(socketAddress);
 #endif

    	if (socketStatus == SOCKET_CLOSED) {
    		if (!resolveHost(addr)) return false;
    		socketAddress.sin_port = htons(port);
			if (bind(socketFd,(struct sockaddr *)&socketAddress,sizeof(socketAddress))) return false;
			if (listen(socketFd,SERVER_BACKLOG)) return false;
			getsockname(socketFd,(struct sockaddr *)&socketAddress,&size);
			socketStatus = SOCKET_LISTENING;
			onServerUp();
			return true;
    	}
    	return false;
    };

    virtual void closeSocket() {
        if (socketStatus != SOCKET_CLOSED) {
           onServerDown();
#ifdef WIN32
           shutdown(socketFd,SD_BOTH);
           closesocket(socketFd);
#else
           shutdown(socketFd,SHUT_RDWR);
           close(socketFd);
#endif
           socketStatus = SOCKET_CLOSED;
        }
    };

    virtual C *acceptConnection() {
        sockaddr_in clientAddress;
        int clientFd;
#ifdef	WIN32
        int size;
#else
        socklen_t size;
#endif
        C *clientSocket=NULL;

        if (socketStatus == SOCKET_LISTENING) {
     	   size = sizeof(clientAddress);
     	   clientFd = accept(socketFd,(struct sockaddr *)&clientAddress,&size);
     	   if (clientFd > 0) {
     		   clientSocket = new C(clientFd,&clientAddress);
     		   clientSocket->onConnect();
     		   onClientConnect(clientSocket);
     	   }
        }
        return clientSocket;
    };

    virtual void onServerUp() = 0;
    virtual void onServerDown() = 0;
    virtual void onClientConnect(C *) = 0;
};

#endif /* ABSTRACTSERVERSOCKET_H_ */
