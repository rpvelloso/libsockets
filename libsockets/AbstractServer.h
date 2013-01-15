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

#ifndef ABSTRACTSERVER_H_
#define ABSTRACTSERVER_H_

#include <string>
#include "Object.h"
#include "AbstractServerSocket.h"

template <class C, class S>
class AbstractServer: public Object {
public:
	AbstractServer() : Object() {
		serverSocket = new S();
	};

	virtual ~AbstractServer() {
		stop();
		delete serverSocket;
	};

	bool start(string addr,unsigned short port) {
		C *clientSocket;

		if (!serverSocket->openSocket(addr,port)) return false;
		while (serverSocket->getSocketStatus() == SOCKET_LISTENING) {
			clientSocket = serverSocket->acceptConnection();
			if (clientSocket) onClientConnect(clientSocket);
		}
		return true;
	};

	void stop() {
		serverSocket->closeSocket();
	};

	virtual void onClientConnect(C *) = 0;
protected:
	S *serverSocket;
};


#endif /* ABSTRACTSERVER_H_ */
