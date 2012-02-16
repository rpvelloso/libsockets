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
#ifndef TECHOSERVER_H_
#define TECHOSERVER_H_

#include <list>
#include "techothread.h"
#include "techolog.h"
#include "techoserversocket.h"

class tEchoThread;

class tEchoServer : public tObject {
public:
	tEchoServer();
	~tEchoServer();
	void Run(const char *, unsigned short);
	void Stop();
	void RemoveThread(tEchoThread *);
	tEchoLog *GetLog();
protected:
	tEchoServerSocket *server_socket;
	list<tEchoThread *> threads;
    tEchoLog *log;
};

#endif
