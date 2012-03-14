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
#ifndef TECHOCLIENTSOCKET_H_
#define TECHOCLIENTSOCKET_H_

#include <libsockets/libsockets.h>
#include "techolog.h"

#define ECHO_BUFLEN 4096

class tEchoClientSocket : public tClientSocket {
public:
	tEchoClientSocket(int, sockaddr_in *);
    ~tEchoClientSocket();
    void setLog(tEchoLog *);
    void onSend(void *, size_t *);
    void onReceive(void *, size_t);
    void onConnect();
    void onDisconnect();
    void processOutput();
protected:
    tEchoLog *log;
    char echoBuffer[ECHO_BUFLEN+1];
    size_t buflen,bufpos;
};

#endif
