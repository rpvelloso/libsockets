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

#ifndef ABSTRACTCLIENTSOCKET_H_
#define ABSTRACTCLIENTSOCKET_H_

#include "AbstractSocket.h"

class AbstractClientSocket : public AbstractSocket {
public:
	AbstractClientSocket();
	AbstractClientSocket(int, sockaddr_in *);
	virtual ~AbstractClientSocket();
    bool openSocket(string, unsigned short);
    void closeSocket();
    int sendData(void *, size_t);
    int sendData(string);
    int receiveData(void *, size_t);

    virtual void onSend(void *, size_t) = 0;
    virtual void beforeSend(void *, size_t &) = 0;
    virtual void onReceive(void *, size_t) = 0;
    virtual void onConnect() = 0;
    virtual void onDisconnect() = 0;
protected:
    ssize_t bytesIn;
    ssize_t bytesOut;
};

#endif /* ABSTRACTCLIENTSOCKET_H_ */
