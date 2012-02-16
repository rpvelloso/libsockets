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
#include "techothread.h"

tEchoThread::tEchoThread(int sd, tEchoServer *o, tEchoClientSocket *s) : tThread(sd) {
	socket = s;
	owner = o;
	log = owner->GetLog();
}

tEchoThread::~tEchoThread() {
	Stop();
}

void tEchoThread::Execute() {
	int len;

	while ((len = socket->Receive(buffer, ECHO_BUFLEN)) > 0) {
		socket->Send(buffer, len);
	}
}

void tEchoThread::OnStart() {
	log->Log("new thread [%x] started.\n",thread_id);
}

void tEchoThread::OnStop() {
	if (GetSelfDestroy()) {
		log->Log("thread [%x] self destroying.\n",thread_id);
		owner->RemoveThread(this);
	}
	log->Log("thread [%x] terminated.\n",thread_id);
	delete socket;
}
