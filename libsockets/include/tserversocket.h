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
#ifndef TSERVERSOCKET_H_
#define TSERVERSOCKET_H_

#ifdef DEVEL_ENV
	#include "tsocket.h"
#else
	#include <libsockets/tsocket.h>
#endif

#define SERVER_BACKLOG 5

template <class C> // tClientSocket derived class
class tServerSocket : public tSocket {
public:
       tServerSocket() : tSocket() {
    	   int reuse=1;

    	   if (setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(int))<0) perror("setsockopt()");
       };

       virtual ~tServerSocket() {};

       int Open(const char *addr, unsigned short port) {
#ifdef WIN32
    	   int size = sizeof(socketAddr);
#else
    	   socklen_t size = sizeof(socketAddr);
#endif

           if (socketStatus == tSocketClosed) {
              if (resolveHost(addr)) return -1;
              socketAddr.sin_port = htons(port);
              if (bind(socketFd,(struct sockaddr *)&socketAddr,sizeof(socketAddr))) return -1;
              if (listen(socketFd,SERVER_BACKLOG)) return -1;
              getsockname(socketFd,(struct sockaddr *)&socketAddr,&size);
              socketStatus = tSocketListening;
              onServerUp();
              return 0;
           }
           return -1;
       };

       void Close() {
           if (socketStatus != tSocketClosed) {
              onServerDown();
#ifdef WIN32
              shutdown(socketFd,SD_BOTH);
              closesocket(socketFd);
#else
              shutdown(socketFd,SHUT_RDWR);
              close(socketFd);
#endif
              socketStatus = tSocketClosed;
           }
       };

       C *Accept() {
           sockaddr_in clientAddr;
           int clientFd;
#ifdef	WIN32
           int size;
#else
           socklen_t size;
#endif
           C *clientSocket=NULL;

           if (socketStatus == tSocketListening) {
        	   size = sizeof(clientAddr);
        	   clientFd = accept(socketFd,(struct sockaddr *)&clientAddr,&size);
        	   if (clientFd > 0) {
        		   clientSocket = new C(clientFd,&clientAddr);
        		   clientSocket->onConnect();
        		   onClientConnect(clientSocket);
        	   }
           }
           return clientSocket;
       };

       virtual void onClientConnect(C *) = 0;
       virtual void onServerUp() = 0;
       virtual void onServerDown() = 0;
};

#endif /* TSERVERSOCKET_H_ */
