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
#include <algorithm>
#include <sstream>
#include <libsockets/libsockets.h>
#include "thttpthread.h"
#include "thttpclientsocket.h"

using namespace std;

tHTTPClientSocket::tHTTPClientSocket(int fd, sockaddr_in *sin) : tClientSocket(fd, sin) {
	log = NULL;
	msgOverflow = 0;
	msgPos = 0;
	lineLength = 0;
	recvSta = tHTTPReceiveHeader;
	httpBody = NULL;
}

tHTTPClientSocket::~tHTTPClientSocket() {
	if (httpBody) free(httpBody);
	Close();
}

void tHTTPClientSocket::SetLog(tHTTPLog *l) {
	log = l;
}

void tHTTPClientSocket::OnSend(void *buf, size_t *size) {
	char s[HTTP_BUFLEN+1];

	memcpy(s,buf,*size);
	s[*size] = 0;
	log->Log("sent to %s: %s\n",GetHostName(),s);
}

void tHTTPClientSocket::OnReceive(void *buf, size_t size) {
	size_t i=0;

	for (;(i<size) && (recvSta == tHTTPReceiveHeader);i++) {
		if (((char *)buf)[i] == ENDL) {
			if (!lineLength) {
				httpHeader[msgPos] = 0x00;
				msgPos = 0;
				if (msgOverflow) {
					msgOverflow = 0;
					LOG("HTTP header buffer overflow.\n");
				} else if (strlen(httpHeader) > 0) ProcessHTTPHeader();
			}
			lineLength = 0;
		} else {
			if (msgPos >= MSG_LEN) {
				msgPos = 0;
				msgOverflow = 1;
			}
			if (((char *)buf)[i] != '\r') {
				httpHeader[msgPos++] = ((char *)buf)[i];
				lineLength++;
			} else httpHeader[msgPos++] = '\n';
		}
	}
	if (recvSta == tHTTPReceiveBody) {
		size_t cpy_len = (size-i)>contentLength?contentLength:(size-i);
		memcpy(&(((char *)httpBody)[bodyPos]),&(((char *)buf)[i]),cpy_len);
		bodyPos += cpy_len;
		if (bodyPos == contentLength) ProcessHTTPBody();
	}
}

void tHTTPClientSocket::OnConnect() {
}

void tHTTPClientSocket::OnDisconnect() {
	log->Log("connection to %s closed.\n",GetHostName());
}

void tHTTPClientSocket::ProcessHTTPHeader() {
	string line, lineAux, parm, msg = httpHeader;
	int i=0;
	stringstream cl;
	string request;

	host = "";
	method = "";
	uri = "";
	query = "";
	httpVersion = "";
	userAgent = "";
	contentType = "";
	boundary = "";
	contentLength = 0;

	do {
		line = stringtok(&msg,"\n\r"); i++;
		lineAux = line;
		if (i == 1) { // request line
			method = stringtok(&line," ");
			request = stringtok(&line," ");
			uri = stringtok(&request,"?");
			query = request;
			httpVersion = line;
		} else { // header lines
			parm = stringtok(&line,": ");
			upper_case(parm);
			if (parm == "HOST") {
				host = line;
			} else if (parm == "USER-AGENT") {
				userAgent = line;
			} else if (parm == "CONTENT-TYPE") {
				contentType = stringtok(&line,"; ");
				stringtok(&line,"=");
				boundary = line;
			} else if (parm == "CONTENT-LENGTH") {
				cl << line;
				cl >> contentLength;
			}
		}
	} while (lineAux != "");

	cout << "Method..: \'" << method << "\'" << endl
	<< "URI.....: \'" << uri << "\'" << endl
	<< "Query...: \'" << query << "\'" << endl
	<< "Version.: \'" << httpVersion << "\'" << endl
	<< "Host....: \'" << host << "\'" << endl
	<< "Agent...: \'" << userAgent << "\'" << endl
	<< "Length..: \'" << contentLength << "\'" << endl
	<< "Type....: \'" << contentType << "\'" << endl
	<< "Boundary: \'" << boundary << "\'" << endl;

	if (contentLength > 0) {
		if (httpBody) {
			free(httpBody);
			httpBody = NULL;
		}
		httpBody = malloc(contentLength+1);
		bodyPos = 0;
		((char *)httpBody)[contentLength] = 0x00;
		recvSta = tHTTPReceiveBody;
	}
}

void tHTTPClientSocket::ProcessHTTPBody() {
	cout << "Body....: " << endl;
	cout << "\'" << (char *)httpBody << "\'" << endl;

	free(httpBody);
	httpBody = NULL;
	recvSta = tHTTPReceiveHeader;
}
