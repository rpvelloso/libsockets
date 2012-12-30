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

#include <list>
#include "AbstractThread.h"
#include "AbstractServer.h"
#include "ClientSocketMultiplexer.h"

template <class C, class S, class T>
class MultiplexedServer: public AbstractServer<C,S> {
public:
	MultiplexedServer(int threadCount) : AbstractServer<C,S>() {
		for (int i=0;i<threadCount;i++) {
			T *t = new T();
			threads.push_back(t);
			t->start();
		}
	};

	virtual ~MultiplexedServer() {
		typename list<T *>::iterator i;

		for (i=threads.begin();i!=threads.end();i++) {
			(*i)->getMultiplexer()->cancelWait();
			while ((*i)->getThreadStatus() == THREAD_RUNNING);
		}
		threads.clear();
	};

	void onClientConnect(C *clientSocket) {
		ClientSocketMultiplexer *multiplexer;
		T *thread;

		if ((thread = getThread())) {
			if ((multiplexer = thread->getMultiplexer())) {
				multiplexer->addSocket(clientSocket);
				return;
			}
		}
		delete clientSocket;
	}

	virtual T *getThread() {
		typename list<T *>::iterator i;
		T *thread = NULL;
		ClientSocketMultiplexer *multiplexer;
		size_t count=~0;

		for (i=threads.begin();i!=threads.end();++i) {
			multiplexer = (*i)->getMultiplexer();
			if (multiplexer) {
				size_t sc = multiplexer->getSocketCount();
				if (sc <= count) {
					thread = (*i);
					count = sc;
				}
			}
		}
		return thread;
	};

	size_t getThreadCount() {
		return threads.size();
	}

protected:
	list<T *> threads;
};

#endif /* SERVER_H_ */
