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

#include "AbstractThread.h"

void *pthread_execute(void *);

AbstractThread::AbstractThread() : Object() {
    threadStatus = THREAD_STOPPED;
    threadId = 0;
}

AbstractThread::~AbstractThread() {
	stop();
}

bool AbstractThread::start() {
    return pthread_create(&threadId, NULL, pthread_execute, (void *)this)==0;
}

void AbstractThread::stop() {
    if (threadStatus == THREAD_RUNNING) {
    	threadStatus = THREAD_STOPPED;
    	pthread_cancel(threadId);
    	onStop();
    }
}

ThreadStatus AbstractThread::getThreadStatus() {
	return threadStatus;
}

pthread_t AbstractThread::getThreadId() {
	return threadId;
}

void *pthread_execute(void *arg) {
	AbstractThread *t = (AbstractThread *)arg;

	pthread_detach(pthread_self());
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	t->threadStatus = THREAD_RUNNING;
	t->onStart();
	t->execute();
	t->onStop();
	delete t;
	pthread_exit(NULL);
	return NULL;
}
