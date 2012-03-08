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

#ifndef TSOCKETMULTIPLEXER_H_
#define TSOCKETMULTIPLEXER_H_

#include <list>
#ifdef WIN32
	#include <winsock2.h>
#else
	#include <sys/select.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef DEVEL_ENV
	#include "tobject.h"
	#include "tmutex.h"
#else
	#include <libsockets/tobject.h>
	#include <libsockets/tmutex.h>
#endif

enum tSocketMultiplexerState {
	tSocketMultiplexerIdle=0,
	tSoscketMultiplexerWaiting
};
 /* TODO: usar pthread_kill ou self-pipe p/ interromper select()
  * apos alterar a lista de sockets */

#define interruptWait() write(ctrlPipe[1],'\0',1)

template <class C>
class tSocketMultiplexer : public tObject {
public:
	tSocketMultiplexer() {
		state = tSocketMultiplexerIdle;
		pipe(ctrlPipe);
	};

	virtual ~tSocketMultiplexer() {
		close(ctrlPipe[0]);
		close(ctrlPipe[1]);
	};

	void addSocket(C *socket) {
		if (C->getBlokingIOState() == tBlocking) C->toggleNonBlockingIO();
		socketListMutex->lock();
		socketList.push_back(C);
		socketListMutex->unlock();
		interruptWait();
	};

	void removeSocket(C *socket) {
		socketListMutex->lock();
		socketList.remove(C);
		socketListMutex->unlock();
		interruptWait();
	};

	int waitForData() {
		fd_set rfds;
		int maxFd=ctrlPipe[0],fd,r=0;
		list<C *>::iterator i;

		state = tSoscketMultiplexerWaiting;
		while (r>=0) {
			FD_ZERO(&rfds);
			socketListMutex->lock();
			for (i=socketList.begin();i!=socketList.end();i++) {
				fd = (*i)->getSocketFd();
				FD_SET(fd,&rfds);
				if (fd>maxFd) maxFd = fd;
			}
			socketListMutex->unlock();
			maxFd++;

			r = select(maxFd, &rfds, NULL, NULL, NULL);
			if (r>0) {
				list<C *> s;
				char c;

				socketListMutex->lock();
				for (i=socketList.begin();i!=socketList.end();i++) {
					fd = (*i)->getSocketFd();
					if (FD_ISSET(fd,&rfds)) s.push_back(*i);
				}
				socketListMutex->unlock();
				for (i=s.begin();i!=s.end();i++) onDataAvailable(*i);
				s.clear();
				if (FD_ISSET(ctrlPipe[0],&rfds)) {
					read(ctrlPipe[0],&c,1);
					if (c) return;
				}
			}
		}
		state = tSocketMultiplexerIdle;
	};

	tSocketMultiplexerState getState() {
		return state;
	};

	void exitWait() {
		write(ctrlPipe[1],'\1',1);
	};

	virtual void onDataAvailable(C *socket) = 0;
protected:
	list<C *> socketList;
	int ctrlPipe[2];
	tSocketMultiplexerState state;
	tMutex *socketListMutex;
};

#endif /* TSOCKETMULTIPLEXER_H_ */
