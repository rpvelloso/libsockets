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

#ifndef SERVER_H_
#define SERVER_H_

#include <set>
#include "AbstractThread.h"
#include "AbstractServer.h"
#include "ClientSocketMultiplexer.h"

template <class C, class S, class T>
class MultiplexedServer: public AbstractServer<C,S> {
public:
	MultiplexedServer(int threadCount) : AbstractServer<C,S>() {
		for (int i=0;i<threadCount;i++) {
			T *t = new T();
			threads.insert(t);
			t->start();
		}
	};

	virtual ~MultiplexedServer() {
		typename set<T *>::iterator i;

		for (i=threads.begin();i!=threads.end();i++) {
			(*i)->getMultiplexer()->cancelWait();
			while ((*i)->getThreadStatus() == THREAD_RUNNING);
		}
		threads.clear();
	};

	void onClientConnect(C *clientSocket) {
		ClientSocketMultiplexer *multiplexer;
		T *thread;

		if (clientSocket) {
			thread = getThread();
			if (thread) {
				multiplexer = thread->getMultiplexer();
				if (multiplexer) multiplexer->addSocket(clientSocket);
			}
		}
	}

	virtual T *getThread() {
		typename set<T *>::iterator i;
		T *thread = NULL;
		ClientSocketMultiplexer *multiplexer;
		size_t count=0xFFFFFFFF;

		for (i=threads.begin();i!=threads.end();i++) {
			multiplexer = (*i)->getMultiplexer();
			if (multiplexer) {
				if (multiplexer->getSocketCount() <= count) {
					thread = (*i);
					count = multiplexer->getSocketCount();
				}
			}
		}
		return thread;
	};

	size_t getThreadCount() {
		return threads.size();
	}

protected:
	set<T *> threads;
};

#endif /* SERVER_H_ */
