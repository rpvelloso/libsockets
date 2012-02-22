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
#include "thttpserversocket.h"

// *** tEchoServerSocket ***
tHTTPServerSocket::tHTTPServerSocket() : tServerSocket<tHTTPClientSocket>() {
	log = NULL;
}

tHTTPServerSocket::~tHTTPServerSocket() {
	Close();
}

void tHTTPServerSocket::SetLog(tHTTPLog *l) {
	log = l;
}

void tHTTPServerSocket::OnClientConnect(tHTTPClientSocket *) {
	log->Log("new client connected to the server.\n");
}

void tHTTPServerSocket::OnServerUp() {
	log->Log("listening for connections on port %d.\n",GetPort());
}

void tHTTPServerSocket::OnServerDown() {
	log->Log("server shutdown.\n");
}
