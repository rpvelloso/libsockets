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
#include "thttpthread.h"
#include "thttpclientsocket.h"

tHTTPClientSocket::tHTTPClientSocket(int fd, sockaddr_in *sin) : tClientSocket(fd, sin) {
	log = NULL;
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
	char s[HTTP_BUFLEN+1];

	memcpy(s,buf,size);
	s[size] = 0;
	log->Log("received from %s: %s\n",GetHostName(),s);
}

void tHTTPClientSocket::OnConnect() {
}

void tHTTPClientSocket::OnDisconnect() {
	log->Log("connection to %s closed.\n",GetHostName());
}
