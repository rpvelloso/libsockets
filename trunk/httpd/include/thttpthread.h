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
#ifndef THTTPTHREAD_H_
#define THTTPTHREAD_H_

#include <libsockets/libsockets.h>
#include "thttpserver.h"
#include "thttpclientsocket.h"
#include "thttplog.h"


#define HTTP_BUFLEN 4096

class tHTTPServer;

class tHTTPThread : public tThread {
public:
	tHTTPThread(int , tHTTPServer *, tHTTPClientSocket *);
	~tHTTPThread();
    void execute();
    void onStart();
    void onStop();
protected:
    tHTTPClientSocket *socket;
    char buffer[HTTP_BUFLEN];
    tHTTPServer *owner;
    tHTTPLog *log;
};

#endif
