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
#include "tftpthread.h"

tFTPThread::tFTPThread(int sd, tFTPServer *o, tFTPClientSocket *s) : tThread(sd) {
	socket = s;
	owner = o;
	log = owner->getLog();
	s->setOwner(o);
}

tFTPThread::~tFTPThread() {
    stop();
}

void tFTPThread::execute() {
	int len;

	while ((len = socket->receive(buffer, RECV_BUFLEN)) > 0);
}

void tFTPThread::onStart() {
	log->log(socket,"new thread [%x] started.\n",threadId);
}

void tFTPThread::onStop() {
	if (getSelfDestroy()) {
		log->log(socket,"thread [%x] self destroying.\n",threadId);
		owner->removeThread(this);
	}
	log->log(socket,"thread [%x] terminated.\n",threadId);
	delete socket;
	owner->setTotalClients(-1);
}
