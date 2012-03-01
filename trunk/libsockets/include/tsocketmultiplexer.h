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

template <class C, class T>
class tSocketMultiplexer : public tObject {
public:
	tSocketMultiplexer() { state = tSocketMultiplexerIdle; };
	virtual ~tSocketMultiplexer() {};
	void addSocket(C *socket) {};
	void removeSocket(int pos) {};
	void removeSocket(C *socket) {};
	int waitForData() {};
	struct timeval getTimeout() { return timeout; };
	void setTimeout(struct timeval t) { memcpy(&timeout,&t,sizeof(struct timeval)); };
	T *getOwner() { return owner; };
	void setOwner(T *o) { owner = o; };
	tSocketMultiplexerState getState() { return state; };

	virtual void onDataAvailable(C *socket) = 0;
	virtual void onTimeout() = 0;
	virtual void onWaitError() = 0;
protected:
	list<int> socketFdList;
	list<C *> socketList;
	fd_set rdfs;
	struct timeval timeout;
	T *owner;
	tSocketMultiplexerState state;
	tMutex *socketListMutex;
};

#endif /* TSOCKETMULTIPLEXER_H_ */
