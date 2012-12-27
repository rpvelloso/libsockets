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

#ifndef LIBSOCKETS_H_
#define LIBSOCKETS_H_

// Base client socket classes
#include <AbstractClientSocket.h>
#include <AbstractMultiplexedClientSocket.h>

// Base server socket class
#include <AbstractServerSocket.h>

// Base thread classes
#include <AbstractThread.h>
#include <AbstractMultiplexerThread.h>

// Base multiplexer class
#include <ClientSocketMultiplexer.h>

// Logger interface
#include <LoggerInterface.h>

// Base server classes
#include <AbstractServer.h>
#include <MultiplexedServer.h>

/*
 * Example basic multiplexed server implementation with 3 threads:
 *
 * 1) class ClientSocket : public AbstractMultiplexedClientSocket { ... };
 * 2) class ServerSocket : public AbstractServerSocket<ClientSocket> { ... };
 * 3) class Thread : public AbstractMultiplexerThread { ... };
 * 4) typedef MultiplexedServer<ClientSocket,ServerSocket,Thread> HTTPServer;
 * 5) HTTPServer *server = new HTTPServer(3);
 * 6) server->start(bind_addr,port); // ex.: bind_addr = "127.0.0.1" or "0.0.0.0", port = 10000
 */

#endif /* LIBSOCKETS_H_ */
