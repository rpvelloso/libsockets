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

#ifndef ECHOSERVERSOCKET_H_
#define ECHOSERVERSOCKET_H_

#include <libsockets.h>
#include "EchoLogger.h"
#include "EchoClientSocket.h"

extern EchoLogger *logger;

class EchoServerSocket : public AbstractServerSocket<EchoClientSocket> {
public:
	EchoServerSocket() : AbstractServerSocket<EchoClientSocket>() {};
	void onServerUp() { logger->LOG("%s\n","SERVER: iniciado."); };
    void onServerDown() { logger->LOG("%s\n","SERVER: terminado."); };
    void onClientConnect(EchoClientSocket *) { logger->LOG("%s\n","SERVER: conexao recebida."); };
};

#endif /* ECHOSERVERSOCKET_H_ */
