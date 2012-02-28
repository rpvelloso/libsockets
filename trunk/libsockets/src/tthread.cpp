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
#include "tthread.h"

void *pthread_execute(void *);

tThread::tThread(int sd) {
	selfDestroy = sd;
    threadStatus = tThreadStopped;
}

tThread::~tThread() {
}

void tThread::start() {
    if (!pthread_create(&threadId, NULL, pthread_execute, (void *)this))
    	pthread_detach(threadId)
    ;
}

void tThread::stop() {
    if (threadStatus == tThreadRunning) {
       if (pthread_cancel(threadId)==0) {
    	   onStop();
    	   threadStatus = tThreadStopped;
       }
    }
}

tThreadStatus tThread::getThreadStatus() {
    return threadStatus;              
}

pthread_t tThread::getThreadId() {
	return threadId;
}

void tThread::setSelfDestroy(int sd) {
	selfDestroy = sd;
}

int tThread::getSelfDestroy() {
	return selfDestroy;
}

void *pthread_execute(void *arg) {
     tThread *t = (tThread *)arg;
     
     t->threadStatus = tThreadRunning;
     t->onStart();
     t->execute();
     t->onStop();
     t->threadStatus = tThreadStopped;
     if (t->getSelfDestroy()) delete t;
     pthread_exit(NULL);
     return NULL;
}
