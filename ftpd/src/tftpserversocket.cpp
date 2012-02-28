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
#include "tftpserversocket.h"

tFTPServerSocket::tFTPServerSocket() : tServerSocket<tFTPClientSocket>() {
	log = NULL;
}

tFTPServerSocket::~tFTPServerSocket() {
	Close();
}

void tFTPServerSocket::setLog(tFTPLog *l) {
	log = l;
}

void tFTPServerSocket::onClientConnect(tFTPClientSocket *c) {
	log->log(c,"new client connected to the server from %s.\n",c->getHostname());
}

void tFTPServerSocket::onServerUp() {
	log->log("listening for connections on port %d.\n",getPort());
}

void tFTPServerSocket::onServerDown() {
	log->log("server shutdown.\n");
}
