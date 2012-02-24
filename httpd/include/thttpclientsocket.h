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
#define HTTP_HDR_LEN 4096

#define LOG(...) log->Log(__VA_ARGS__)

enum tHTTPRequestState {
	tHTTPReceiveHeader=0,
	tHTTPReceiveBody,
	tHTTPProcessRequest
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
    void ProcessHTTPRequest();

    void GET();
    void OPTIONS();
    int  HEAD();
    void POST();
    void PUT();
    void DEL();
    void TRACE();
    void CONNECT();

    void ReplyServer();
    void ReplyDate();
    void Reply501();
    void Reply404();

protected:
    tHTTPLog *log;
    char httpHeader[HTTP_HDR_LEN];
    void *httpBody;
    int hdrPos,msgOverflow,lineLength;
    size_t bodyPos;
    tHTTPRequestState reqState;
    string userAgent,host,method,uri,query,contentType,boundary,httpVersion;
    unsigned long int contentLength;
};

#endif
