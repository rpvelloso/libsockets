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
#ifndef THTTPCLIENTSOCKET_H_
#define THTTPCLIENTSOCKET_H_

#include <string>
#include <sstream>
#include <libsockets/libsockets.h>
#include "thttplog.h"

#define ENDL '\n'
#define MSG_LEN 4096

#define LOG(...) log->Log(__VA_ARGS__)

enum tHTTPMessageReceiveState {
	tHTTPReceiveHeader=0,
	tHTTPReceiveBody
};

class tHTTPClientSocket : public tClientSocket {
public:
	tHTTPClientSocket(int, sockaddr_in *);
    ~tHTTPClientSocket();
    void SetLog(tHTTPLog *);
    void OnSend(void *, size_t *);
    void OnReceive(void *, size_t);
    void OnConnect();
    void OnDisconnect();
    void ProcessHTTPHeader();
    void ProcessHTTPBody();
protected:
    tHTTPLog *log;
    char http_message[MSG_LEN];
    void *http_body;
    int msg_pos,msg_overflow,lnlen;
    size_t body_pos;
    tHTTPMessageReceiveState recv_sta;
    string userAgent,host,action,uri;
    unsigned long int contentLength;
};

#endif
