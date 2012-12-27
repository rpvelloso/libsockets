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

//TODO: arrumar passagem do parametro 'rootDir', fazer get/set nas classes server e client

#include <signal.h>
#include <stdlib.h>
#include <libsockets.h>
#include "HTTPClientSocket.h"
#include "HTTPServerSocket.h"
#include "HTTPThread.h"

using namespace std;

typedef MultiplexedServer<HTTPClientSocket,HTTPServerSocket,HTTPThread> HTTPServer;
static HTTPServer *server=NULL;

void signalHandler(int sig) {
	if ((sig == SIGINT)		||
		(sig == SIGSTOP)	||
		(sig == SIGTERM)	||
		(sig == SIGQUIT)) {
		if (server) server->stop();
	}
}

void printUsage(char *p) {
	cout << "usage: "<<p<<" [-a bind_addr] [-p bind_port]"<<endl;
	cout << "-a bind_addr: the IP address to bind the server to. Default 127.0.0.1"<<endl;
	cout << "-p bind_port: the server port number. Default 80"<<endl;
	cout << "-r dir: document root directory, must by an absolute path. Default \"/\"."<<endl;
	exit(-1);
}

#ifdef WIN32
int WinSocketStartup() {
     WSADATA info;
     return WSAStartup(MAKEWORD(2,0), &info);
}
#endif

string rootDir="/";

int main(int argc, char **argv) {
	int opt;
	string bindAddr="127.0.0.1";
	unsigned short bindPort=80;

	while ((opt = getopt(argc, argv, "a:p:r:h")) != -1) {
		switch (opt) {
		case 'a':
			bindAddr = optarg;
			break;
		case 'p':
			bindPort = atoi(optarg);
			break;
		case 'r':
			rootDir = optarg;
			if (rootDir[rootDir.length()-1]!='/') rootDir = rootDir + '/';
			break;
		case 'h':
		default:
	    	printUsage(argv[0]);
	    break;
		}
	}

	if (rootDir.empty() || (bindPort == 0) ||
		bindAddr.empty() || (rootDir[0] != '/')) {
    	printUsage(argv[0]);
	} else {
#ifdef WIN32
		WinSocketStartup();
#endif
    	server = new HTTPServer(3);

    	signal(SIGINT,signalHandler);
    	signal(SIGTERM,signalHandler);
    	signal(SIGPIPE, SIG_IGN);
    	signal(SIGHUP,signalHandler);
    	signal(SIGSTOP,signalHandler);
    	signal(SIGQUIT,signalHandler);

		server->start(bindAddr,bindPort);
		delete server;
	}
	exit(0);
}
