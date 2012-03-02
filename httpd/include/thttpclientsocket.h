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

#define LOG(...) log->log(__VA_ARGS__)

enum tHTTPRequestState {
	tHTTPReceiveHeader=0,
	tHTTPReceiveBody,
	tHTTPProcessRequest
};

class tHTTPServer;

class tHTTPClientSocket : public tClientSocket {
public:
	tHTTPClientSocket(int, sockaddr_in *);
    ~tHTTPClientSocket();
    void setLog(tHTTPLog *);
    void onSend(void *, size_t *);
    void onReceive(void *, size_t);
    void onConnect();
    void onDisconnect();
    void processHttpHeader();
    void processHttpBody();
    void processHttpRequest();
    tHTTPServer *getOwner();
    void setOwner(tHTTPServer *);

    void GET();
    void OPTIONS();
    int  HEAD();
    void POST();
    void PUT();
    void DEL();
    void TRACE();
    void CONNECT();

    void replyServer();
    void replyDate();
    void reply403Forbidden();
    void reply404NotFound();
    void reply500InternalError();
    void reply501NotImplemented();

protected:
    tHTTPLog *log;
    char httpHeader[HTTP_HDR_LEN];
    int hdrPos,msgOverflow,lineLength;
    ssize_t bodyPos;
    tHTTPRequestState reqState;
    string userAgent,host,method,uri,scriptName,
    query,contentType,boundary,httpVersion,referer,cookie;
    ssize_t contentLength;
    tHTTPServer *owner;
    FILE *tmpPostData;
    FILE *tmpRespData;

    void CGICall();
};

#endif
