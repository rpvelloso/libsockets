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
	msg_overflow = 0;
	msg_pos = 0;
	lnlen = 0;
	recv_sta = tHTTPReceiveHeader;
	http_body = NULL;
}

tHTTPClientSocket::~tHTTPClientSocket() {
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

	for (;(i<size) && (recv_sta == tHTTPReceiveHeader);i++) {
		if (((char *)buf)[i] == ENDL) {
			if (!lnlen) {
				http_message[msg_pos] = 0x00;
				msg_pos = 0;
				if (msg_overflow) {
					msg_overflow = 0;
					LOG("HTTP message buffer overflow.\n");
				} else if (strlen(http_message) > 0) ProcessHTTPHeader();
			}
			lnlen = 0;
		} else {
			if (msg_pos >= MSG_LEN) {
				msg_pos = 0;
				msg_overflow = 1;
			}
			if (((char *)buf)[i] != '\r') {
				http_message[msg_pos++] = ((char *)buf)[i];
				lnlen++;
			} else http_message[msg_pos++] = '\n';
		}
	}
	if (recv_sta == tHTTPReceiveBody) {
		size_t cpy_len = (size-i)>contentLength?contentLength:(size-i);
		memcpy(&(((char *)http_body)[body_pos]),&(((char *)buf)[i]),cpy_len);
		body_pos += cpy_len;
		if (body_pos == contentLength) ProcessHTTPBody();
	}
}

void tHTTPClientSocket::OnConnect() {
}

void tHTTPClientSocket::OnDisconnect() {
	log->Log("connection to %s closed.\n",GetHostName());
}

void tHTTPClientSocket::ProcessHTTPHeader() {
	string line, parm, msg = http_message;
	int i=0;
	stringstream cl;

	contentLength = 0;
	host = "";
	do {
		line = stringtok(&msg,"\n"); i++;
		if (i == 1) { // request line
			action = stringtok(&line," ");
			uri = stringtok(&line," ");
		} else { // header lines
			parm = stringtok(&line,": ");
			upper_case(parm);
			if (parm == "HOST") {
				host = line;
			} else if (parm == "CONTENT-LENGTH") {
				cl << line;
				cl >> contentLength;
			}
		}
	} while (line!="");
	cout << "Action: \'" << action << "\'" << endl;
	cout << "URI: \'" << uri << "\'" << endl;
	cout << "Host: \'" << host << "\'" << endl;
	cout << "Length: \'" << contentLength << "\'" << endl;
	if (contentLength > 0) {
		if (http_body) {
			free(http_body);
			http_body = NULL;
		}
		http_body = malloc(contentLength+1);
		body_pos = 0;
		((char *)http_body)[contentLength] = 0x00;
		recv_sta = tHTTPReceiveBody;
	}
}

void tHTTPClientSocket::ProcessHTTPBody() {
	cout << "HTTP Body: " << endl;
	cout << "\'" << (char *)http_body << "\'" << endl;
	free(http_body);
	http_body = NULL;
	recv_sta = tHTTPReceiveHeader;
}
