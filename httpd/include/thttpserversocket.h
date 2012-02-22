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
#ifndef THTTPSERVERSOCKET_H_
#define THTTPSERVERSOCKET_H_

#include "thttpclientsocket.h"
#include "thttplog.h"

class tHTTPServerSocket : public tServerSocket<tHTTPClientSocket> {
public:
	tHTTPServerSocket();
    ~tHTTPServerSocket();
    void SetLog(tHTTPLog *);
    void OnClientConnect(tHTTPClientSocket *);
    void OnServerUp();
    void OnServerDown();

protected:
    tHTTPLog *log;
};

#endif
