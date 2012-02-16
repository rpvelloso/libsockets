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
	log = owner->GetLog();
	s->SetOwner(o);
}

tFTPThread::~tFTPThread() {
    Stop();
}

void tFTPThread::Execute() {
	int len;

	while ((len = socket->Receive(buffer, RECV_BUFLEN)) > 0);
}

void tFTPThread::OnStart() {
	log->Log(socket,"new thread [%x] started.\n",thread_id);
}

void tFTPThread::OnStop() {
	if (GetSelfDestroy()) {
		log->Log(socket,"thread [%x] self destroying.\n",thread_id);
		owner->RemoveThread(this);
	}
	log->Log(socket,"thread [%x] terminated.\n",thread_id);
	delete socket;
	owner->SetTotalClients(-1);
}
