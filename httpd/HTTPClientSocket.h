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

#include <cstdlib>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <fcntl.h>
#include <libsockets.h>
#include "CGIControlThread.h"

class HTTPServerSocket;

enum HTTPRequestState {
	HTTP_RECEIVE_HEADER,
	HTTP_RECEIVE_BODY,
	HTTP_PROCESS_REQUEST,
	HTTP_REQUEST_ENDED
};

enum HTTPReply {
	REPLY_403_FORBIDDEN=403,
	REPLY_404_NOT_FOUND=404,
	REPLY_500_INTERNAL_SERVER_ERROR=500,
	REPLY_501_NOT_IMPLEMENTED=501
};

#define upperCase(s) std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::toupper)
#define lowerCase(s) std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::tolower)

#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"

// class to enable access to fstream file fd
class fdbuf : public filebuf {
public:
	int fd() { return _M_file.fd(); }
};

// tmpfile() C++ replacement
class tmpfstream : public fstream {
public:
	void tmp_open() {
		if (!(this->is_open())) {
			char *fn = tempnam(NULL,NULL);

			tmpfilename = fn;
			free(fn);
			this->open(tmpfilename.c_str(),
					fstream::in		|
					fstream::out	|
					fstream::trunc	|
					fstream::binary);
		}
	};

	void tmp_close() {
		if (this->is_open()) {
			this->close();
			remove(tmpfilename.c_str());
		}
	};

	void tmp_reopen(ios_base::openmode m) {
		if (this->is_open()) {
			this->close();
			this->open(tmpfilename.c_str(),m);
		}
	};

private:
	string tmpfilename;
};

class CGIThread;

class HTTPClientSocket: public AbstractMultiplexedClientSocket {
public:
	HTTPClientSocket();
	HTTPClientSocket(int, sockaddr_in*, SSL_CTX *);
	virtual ~HTTPClientSocket();

	void onSend(void *, size_t);
	void beforeSend(void *, size_t &);
	void onReceive(void *, size_t);
    void onConnect();
	void onDisconnect();

	void onCGIEnd();

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

	void reply(HTTPReply);
	void replyServer();
	void replyDate();

	void GET();
	bool HEAD();
	void POST();
	void OPTIONS();
	void PUT();
	void DEL();
	void TRACE();
	void CONNECT();

	void executeCGI();

	void setOutputBuffer(iostream *);
	void restoreOutputBuffer();

	string stringTok(string &, string);
	string unescapeUri(string);

	HTTPRequestState requestState;
	stringstream inputBuffer;
	int headerLineNo;
	ssize_t contentLength;
	string method,requestURI,scriptFileName,scriptName,query,httpVersion,
		host,userAgent,contentType,boundary,referer,cookie,connection;
	fstream file;
	tmpfstream CGIOutput,CGIInput;
	iostream *saveOutputBuffer;
	string documentRoot;

	HTTPServerSocket *serverSocket;

	pid_t CGIPID;

	LoggerInterface *logger;
};

#endif /* HTTPCLIENTSOCKET_H_ */
