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

#include <csignal>
#include "HTTPClientSocket.h"
#include "CGIControlThread.h"

CGIControlThread::CGIControlThread() : AbstractThread() {
	mutex = new Mutex();
}

CGIControlThread::~CGIControlThread() {
	delete mutex;
	socketmap.clear();
}

void CGIControlThread::onStart() {
}

void CGIControlThread::onStop() {
}

void CGIControlThread::execute() {
	pid_t pid;
	int ret,sig;
	sigset_t waitset;

	sigemptyset(&waitset);
	sigaddset(&waitset,SIGCHLD); // need to block this signal in the main thread!!!

	while (true) {
		sigwait(&waitset,&sig);
		while ((pid = wait(&ret)) > 0) {
			HTTPClientSocket *s = removePID(pid);
			if (s) s->onCGIEnd();
		}
	}
}

void CGIControlThread::addPID(pid_t p, HTTPClientSocket* s) {
	mutex->lock();
	socketmap[p]=s;
	mutex->unlock();
}

HTTPClientSocket* CGIControlThread::removePID(pid_t p) {
	HTTPClientSocket *s;

	mutex->lock();
	s = socketmap[p];
	socketmap.erase(p);
	mutex->unlock();

	return s;
}
