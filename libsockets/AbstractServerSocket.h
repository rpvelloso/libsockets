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

#include <iostream>
#include <cstdio>
#include <arpa/inet.h>
#include "AbstractSocket.h"

#define SERVER_BACKLOG 5

template <class C>
class AbstractServerSocket: public AbstractSocket {
public:
	AbstractServerSocket() : AbstractSocket() {
 	   int reuse=1;

 	   if (setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(int))<0) perror("setsockopt()");
 	   certificateFile = keyFile = "";
	};

	virtual ~AbstractServerSocket() {};

	virtual void setSSLPEMFiles(string cert, string key) {
		certificateFile = cert;
		keyFile = key;
	}

    virtual bool openSocket(string addr, unsigned short port, bool ssl=false) {
    	socklen_t size = sizeof(socketAddress);

    	if (socketStatus == SOCKET_CLOSED) {
    		if (!resolveHost(addr)) return false;
    		socketAddress.sin_port = htons(port);
			if (bind(socketFd,(struct sockaddr *)&socketAddress,sizeof(socketAddress))) return false;
			if (listen(socketFd,SERVER_BACKLOG)) return false;
			getsockname(socketFd,(struct sockaddr *)&socketAddress,&size);
			socketStatus = SOCKET_LISTENING;
			if (ssl && (certificateFile != "") && (keyFile != "")) {
				if (!(sslContext = SSL_CTX_new(SSLv23_server_method()))) {
					ERR_print_errors_fp(stderr);
					return false;
				}
			    if (SSL_CTX_use_certificate_file(sslContext,certificateFile.c_str(),SSL_FILETYPE_PEM) <= 0) {
			        ERR_print_errors_fp(stderr);
					return false;
			    }
			    if (SSL_CTX_use_PrivateKey_file(sslContext,keyFile.c_str(),SSL_FILETYPE_PEM) <= 0) {
			        ERR_print_errors_fp(stderr);
					return false;
			    }
			    if (!SSL_CTX_check_private_key(sslContext)) {
			        fprintf(stderr, "Private key does not match the public certificate\n");
					return false;
			    }
			}
			onServerUp();
			return true;
    	}
    	return false;
    };

    virtual void closeSocket() {
        if (socketStatus != SOCKET_CLOSED) {
           onServerDown();
           shutdown(socketFd,SHUT_RDWR);
           close(socketFd);
           if (sslContext) {
        	   SSL_CTX_free(sslContext);
        	   sslContext = NULL;
           }
           socketStatus = SOCKET_CLOSED;
        }
    };

    virtual C *acceptConnection() {
        sockaddr_in clientAddress;
        int clientFd;
        socklen_t size;
        C *clientSocket=NULL;

        if (socketStatus == SOCKET_LISTENING) {
     	   size = sizeof(clientAddress);
     	   clientFd = accept(socketFd,(struct sockaddr *)&clientAddress,&size);
     	   if (clientFd > 0) {
     		   setCloseOnExec(clientFd);
     		   clientSocket = new C(clientFd,&clientAddress,sslContext);
     		   clientSocket->onConnect();
     		   onClientConnect(clientSocket);
     	   }
        }
        return clientSocket;
    };

    virtual void onServerUp() = 0;
    virtual void onServerDown() = 0;
    virtual void onClientConnect(C *) = 0;
private:
    string certificateFile,keyFile;
};

#endif /* ABSTRACTSERVERSOCKET_H_ */
