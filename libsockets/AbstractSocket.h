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

#ifndef ABSTRACTSOCKET_H_
#define ABSTRACTSOCKET_H_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Object.h"

enum SocketStatus {
     SOCKET_OPENED,
     SOCKET_CLOSED,
     SOCKET_LISTENING
};

class AbstractSocket : public Object {
public:
	AbstractSocket();
	virtual ~AbstractSocket();
    virtual bool openSocket(string, unsigned short) = 0;
    virtual void closeSocket() = 0;
    bool resolveHost(string);
    string getHostname();
    unsigned short getPort();
    string getIPAddress();
    bool setLinger(int,int);
    int getLinger();
    bool setNonBlocking(bool);
    bool getNonBlocking();
    int getSocketFd();
    SocketStatus getSocketStatus();

    static bool setNonBlocking(int, bool);
    static bool setCloseOnExec(int);
protected:
    int socketFd;
    SocketStatus socketStatus;
    sockaddr_in socketAddress;
    string hostname;
    bool nonBlocking;
    int linger;

};


#endif /* ABSTRACTSOCKET_H_ */
