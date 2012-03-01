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
#ifndef TCLIENTSOCKET_H_
#define TCLIENTSOCKET_H_

#ifdef DEVEL_ENV
	#include "tsocket.h"
#else
	#include <libsockets/tsocket.h>
#endif

class tClientSocket : public tSocket {
public:
       tClientSocket();
       tClientSocket(int, sockaddr_in *);
       virtual ~tClientSocket();
       int Open(const char *, unsigned short);
       virtual void Close();
       int Send(void *, size_t);
       int Send(string);
       int receive(void *, size_t);
       ssize_t sendFile(const char *, off_t *, ssize_t);
       ssize_t sendFile(FILE *, off_t *, ssize_t);
       int setReceiveTimeout(struct timeval);
       int setSendTimeout(struct timeval);

       virtual void onSend(void *, size_t *) = 0;
       virtual void onReceive(void *, size_t) = 0;
       virtual void onConnect() = 0;
       virtual void onDisconnect() = 0;
protected:
       ssize_t bytesIn;
       ssize_t bytesOut;
};

#endif /* TCLIENTSOCKET_H_ */
