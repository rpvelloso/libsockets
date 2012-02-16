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
#ifndef TECHOTHREAD_H_
#define TECHOTHREAD_H_

#include <libsockets/libsockets.h>
#include "techoserver.h"
#include "techoclientsocket.h"
#include "techolog.h"


#define ECHO_BUFLEN 4096

class tEchoServer;

class tEchoThread : public tThread {
public:
	tEchoThread(int , tEchoServer *, tEchoClientSocket *);
	~tEchoThread();
    void Execute();
    void OnStart();
    void OnStop();
protected:
    tEchoClientSocket *socket;
    char buffer[ECHO_BUFLEN];
    tEchoServer *owner;
    tEchoLog *log;
};

#endif
