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

    	   if (setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(int))<0) perror("setsockopt()");
       };

       virtual ~tServerSocket() {};

       int Open(const char *addr, unsigned short port) {
#ifdef WIN32
    	   int size = sizeof(socket_addr);
#else
    	   socklen_t size = sizeof(socket_addr);
#endif

           if (socket_status == tSocketClosed) {
              if (ResolveHost(addr)) return -1;
              socket_addr.sin_port = htons(port);
              if (bind(socket_fd,(struct sockaddr *)&socket_addr,sizeof(socket_addr))) return -1;
              if (listen(socket_fd,SERVER_BACKLOG)) return -1;
              getsockname(socket_fd,(struct sockaddr *)&socket_addr,&size);
              socket_status = tSocketListening;
              OnServerUp();
              return 0;
           }
           return -1;
       };

       void Close() {
           if (socket_status != tSocketClosed) {
              OnServerDown();
#ifdef WIN32
              shutdown(socket_fd,SD_BOTH);
              closesocket(socket_fd);
#else
              shutdown(socket_fd,SHUT_RDWR);
              close(socket_fd);
#endif
              socket_status = tSocketClosed;
           }
       };

       C *Accept() {
           sockaddr_in client_addr;
           int client_fd;
#ifdef	WIN32
           int size;
#else
           socklen_t size;
#endif
           C *clientsocket=NULL;

           if (socket_status == tSocketListening) {
        	   size = sizeof(client_addr);
        	   client_fd = accept(socket_fd,(struct sockaddr *)&client_addr,&size);
        	   if (client_fd > 0) {
        		   clientsocket = new C(client_fd,&client_addr);
        		   clientsocket->OnConnect();
        		   OnClientConnect(clientsocket);
        	   }
           }
           return clientsocket;
       };

       virtual void OnClientConnect(C *) = 0;
       virtual void OnServerUp() = 0;
       virtual void OnServerDown() = 0;
};

#endif /* TSERVERSOCKET_H_ */
