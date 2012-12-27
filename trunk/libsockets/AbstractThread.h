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

#ifndef ABSTRACTTHREAD_H_
#define ABSTRACTTHREAD_H_

#include <pthread.h>
#include "Object.h"

enum ThreadStatus {
     THREAD_STOPPED,
     THREAD_RUNNING,
     THREAD_CANCELED
};

class AbstractThread: public Object {
friend void *pthread_execute(void *);
public:
	AbstractThread();
	virtual ~AbstractThread();
    int start();
    void stop();
    ThreadStatus getThreadStatus();
    pthread_t getThreadId();

    virtual void onStart() = 0;
    virtual void execute() = 0;
    virtual void onStop() = 0;
protected:
    pthread_t threadId;
    ThreadStatus threadStatus;
};

#endif /* ABSTRACTTHREAD_H_ */
