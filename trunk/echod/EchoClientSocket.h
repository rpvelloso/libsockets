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

#ifndef ECHOCLIENTSOCKET_H_
#define ECHOCLIENTSOCKET_H_

#include <libsockets.h>

class EchoClientSocket : public AbstractMultiplexedClientSocket {
public:
	EchoClientSocket();
	EchoClientSocket(int, sockaddr_in*);

	void onSend(void *, size_t);
	void beforeSend(void *, size_t *);
    void onReceive(void *, size_t);
    void onConnect();
	void onDisconnect();
};


#endif /* ECHOCLIENTSOCKET_H_ */
