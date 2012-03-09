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
#ifndef TECHOSOCKETMULTIPLEXER_H_
#define TECHOSOCKETMULTIPLEXER_H_

#include <errno.h>
#include <libsockets/libsockets.h>
#include "techoclientsocket.h"

#ifdef WIN32
	#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

#define ECHO_BUFLEN 4096

class tEchoSocketMultiplexer : public tSocketMultiplexer<tEchoClientSocket> {
public:
	tEchoSocketMultiplexer() : tSocketMultiplexer() {};

	~tEchoSocketMultiplexer() {};

	void onDataAvailable(tEchoClientSocket *socket) {
		int len;

		if ((len = socket->receive(buffer, ECHO_BUFLEN)) > 0) {
			socket->Send(buffer, len);
		} else {
			if ((errno != EINTR) && (errno != EWOULDBLOCK)) removeSocket(socket);
		}
	};
protected:
    char buffer[ECHO_BUFLEN];
};

#endif /* TECHOSOCKETMULTIPLEXER_H_ */
