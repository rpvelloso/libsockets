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
       int Receive(void *, size_t);
       ssize_t SendFile(const char *, off_t *, ssize_t);

       virtual void OnSend(void *, size_t *) = 0;
       virtual void OnReceive(void *, size_t) = 0;
       virtual void OnConnect() = 0;
       virtual void OnDisconnect() = 0;
protected:
       ssize_t bytes_in;
       ssize_t bytes_out;
};

#endif /* TCLIENTSOCKET_H_ */
