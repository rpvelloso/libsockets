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

#include <cstdlib>
#include <csignal>
#include <libsockets.h>
#include "EchoClientSocket.h"
#include "EchoServerSocket.h"
#include "EchoThread.h"
#include "EchoLogger.h"

using namespace std;

typedef MultiplexedServer<EchoClientSocket,EchoServerSocket,EchoThread> EchoServer;
static EchoServer *server=NULL;
EchoLogger *logger = new EchoLogger("/dev/stderr");

void signalHandler(int sig) {
	if ((sig == SIGINT)		||
		(sig == SIGSTOP)	||
		(sig == SIGTERM)	||
		(sig == SIGQUIT)) {
		if (server) server->stop();
	}
}

#ifdef WIN32
int WinSocketStartup() {
     WSADATA info;
     return WSAStartup(MAKEWORD(2,0), &info);
}
#endif

int main(int argc, char **argv) {
#ifdef WIN32
	WinSocketStartup();
#endif

	logger->openLog();

	server = new EchoServer(3);

	signal(SIGINT,signalHandler);
	signal(SIGTERM,signalHandler);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP,signalHandler);
	signal(SIGSTOP,signalHandler);
	signal(SIGQUIT,signalHandler);

	server->start("0.0.0.0",10000);
	delete server;
	delete logger;
	exit(0);
}

