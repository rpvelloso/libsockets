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
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#ifdef WIN32
	#include <winsock2.h>
#else
	#include <sys/select.h>
#endif
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

static int INTR_WAIT = 0x00;
static int EXIT_WAIT = 0x01;

#ifdef WIN32
	#define pipe(p) _pipe(p,256, _O_BINARY)
#endif

#define interruptWait() write(ctrlPipe[1],(void *)&INTR_WAIT,1)

template <class C> // tClientSocket derived class
class tSocketMultiplexer : public tObject {
public:
	tSocketMultiplexer() : tObject() {
		state = tSocketMultiplexerIdle;
		socketListMutex = new tMutex();
		pipe(ctrlPipe);
	};

	virtual ~tSocketMultiplexer() {
		typename list<C *>::iterator i;

		stopWaiting();
		while (state != tSocketMultiplexerIdle);

		close(ctrlPipe[1]);
		close(ctrlPipe[0]);

		socketListMutex->lock();
		for (i=socketList.begin();i!=socketList.end();i++) {
			delete (*i);
		}
		socketList.clear();
		socketListMutex->unlock();
		delete socketListMutex;
	};

	void addSocket(C *socket) {
		if (socket->getBlockingIOState() == tBlocking) socket->toggleNonBlockingIO();
		socketListMutex->lock();
		socketList.push_back(socket);
		socketListMutex->unlock();
		interruptWait();
	};

	void removeSocket(C *socket) {
		socketListMutex->lock();
		socketList.remove(socket);
		socketListMutex->unlock();
		interruptWait();
		delete socket;
	};

	int waitForData() {
		fd_set rfds,wfds;
		int maxFd=ctrlPipe[0],fd,r=0;
		typename list<C *>::iterator i;

		state = tSoscketMultiplexerWaiting;
		while (r>=0) {
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_SET(ctrlPipe[0],&rfds);
			socketListMutex->lock();
			for (i=socketList.begin();i!=socketList.end();i++) {
				fd = (*i)->getSocketFd();
				FD_SET(fd,&rfds);
				if ((*i)->hasOutput()) FD_SET(fd,&wfds);
				if (fd>maxFd) maxFd = fd;
			}
			socketListMutex->unlock();

			r = select(maxFd + 1, &rfds, &wfds, NULL, NULL);
			if (r>0) {
				list<C *> rs,ws;
				char c;

				socketListMutex->lock();
				for (i=socketList.begin();i!=socketList.end();i++) {
					fd = (*i)->getSocketFd();
					if (FD_ISSET(fd,&wfds)) ws.push_back(*i); // half-duplex
					else if (FD_ISSET(fd,&rfds)) rs.push_back(*i);
				}
				socketListMutex->unlock();
				for (i=ws.begin();i!=ws.end();i++) onOutputAvailable(*i);
				for (i=rs.begin();i!=rs.end();i++) onInputAvailable(*i);
				rs.clear();
				ws.clear();
				if (FD_ISSET(ctrlPipe[0],&rfds)) {
					read(ctrlPipe[0],&c,1);
					if (c) break;
				}
			} else {
				if (errno == EINTR) r = 0;
			}
		}
		state = tSocketMultiplexerIdle;
	};

	tSocketMultiplexerState getState() {
		return state;
	};

	void stopWaiting() {
		write(ctrlPipe[1],(const void *)&EXIT_WAIT,1);
	};

	size_t getSocketCount() { return socketList.size(); };
	virtual void onInputAvailable(C *socket) = 0;
	virtual void onOutputAvailable(C *socket) = 0;
protected:
	list<C *> socketList;
	int ctrlPipe[2];
	tSocketMultiplexerState state;
	tMutex *socketListMutex;
};

#endif /* TSOCKETMULTIPLEXER_H_ */
