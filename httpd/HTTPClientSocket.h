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

#ifndef HTTPCLIENTSOCKET_H_
#define HTTPCLIENTSOCKET_H_

#include <sstream>
#include <fstream>
#include <libsockets.h>

class HTTPServerSocket;

enum HTTPRequestState {
	HTTP_RECEIVE_HEADER,
	HTTP_RECEIVE_BODY,
	HTTP_PROCESS_REQUEST,
	HTTP_REQUEST_ENDED
};

class CGIThread;

class HTTPClientSocket: public AbstractMultiplexedClientSocket {
friend class CGIThread;
public:
	HTTPClientSocket();
	HTTPClientSocket(int, sockaddr_in*);
	virtual ~HTTPClientSocket();

	void onSend(void *, size_t);
    void onReceive(void *, size_t);
    void onConnect();
	void onDisconnect();

	HTTPServerSocket* getServerSocket();
	void setServerSocket(HTTPServerSocket *);
	string getDocumentRoot();
	void setDocumentRoot(string);
	LoggerInterface* getLogger();
	void setLogger(LoggerInterface *);

private:
	void processInput();
	void processRequest();
	int checkURI();
	void log(const char *, ...);

	void reply(int);
	void replyServer();
	void replyDate();

	void GET();
	int  HEAD();
	void POST();
	void OPTIONS();
	void PUT();
	void DEL();
	void TRACE();
	void CONNECT();

	void CGICall();

	void setOutputBuffer(iostream *);
	void restoreOutputBuffer();

	string stringTok(string &, string);
	string unescapeUri(string);

	HTTPRequestState requestState;
	stringstream inputBuffer;
	int headerLineNo;
	ssize_t contentLength;
	string method,request,scriptName,URI,query,httpVersion,
		host,userAgent,contentType,boundary,referer,cookie,connection;
	fstream file,CGIOutput,CGIInput;
	iostream *saveOutputBuffer;
	string documentRoot;

	HTTPServerSocket *serverSocket;

	pid_t CGIPid;

	LoggerInterface *logger;
};

#endif /* HTTPCLIENTSOCKET_H_ */
