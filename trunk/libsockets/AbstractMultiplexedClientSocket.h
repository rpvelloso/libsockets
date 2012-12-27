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

#ifndef ABSTRACTMULTIPLEXEDCLIENTSOCKET_H_
#define ABSTRACTMULTIPLEXEDCLIENTSOCKET_H_

#include <iostream>
#ifdef WIN32
#include <winsock2.h>
#endif
#include "AbstractClientSocket.h"

class ClientSocketMultiplexer;

class AbstractMultiplexedClientSocket: public AbstractClientSocket {
public:
	AbstractMultiplexedClientSocket();
	AbstractMultiplexedClientSocket(int, sockaddr_in *);
	virtual ~AbstractMultiplexedClientSocket();
	bool hasDataToSend();
    void sendBufferedData(void *, streamsize);
    void sendBufferedData(string);
    void commitBuffer();
    void transmitBuffer();
    void setMultiplexer(ClientSocketMultiplexer *);
protected:
    ClientSocketMultiplexer *multiplexer;
    iostream *outputBuffer;
};

#endif /* ABSTRACTMULTIPLEXEDCLIENTSOCKET_H_ */
