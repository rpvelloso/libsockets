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
	self_destroy = sd;
    thread_status = tThreadStopped;
}

tThread::~tThread() {
}

void tThread::Start() {
    if (!pthread_create(&thread_id, NULL, pthread_execute, (void *)this))
    	pthread_detach(thread_id)
    ;
}

void tThread::Stop() {
    if (thread_status == tThreadRunning) {
       if (pthread_cancel(thread_id)==0) {
    	   OnStop();
    	   thread_status = tThreadStopped;
       }
    }
}

tThreadStatus tThread::GetThreadStatus() {
    return thread_status;              
}

pthread_t tThread::GetThreadId() {
	return thread_id;
}

void tThread::SetSelfDestroy(int sd) {
	self_destroy = sd;
}

int tThread::GetSelfDestroy() {
	return self_destroy;
}

void *pthread_execute(void *arg) {
     tThread *t = (tThread *)arg;
     
     t->thread_status = tThreadRunning;
     t->OnStart();
     t->Execute();
     t->OnStop();
     t->thread_status = tThreadStopped;
     if (t->GetSelfDestroy()) delete t;
     pthread_exit(NULL);
     return NULL;
}
