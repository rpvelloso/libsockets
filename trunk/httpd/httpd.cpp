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
#include "HTTPClientSocket.h"
#include "HTTPServerSocket.h"
#include "HTTPThread.h"

using namespace std;

class HTTPServer : public MultiplexedServer<HTTPClientSocket,HTTPServerSocket,HTTPThread> {
public:
	HTTPServer(string dr, int tc) : MultiplexedServer<HTTPClientSocket,HTTPServerSocket,HTTPThread>(tc) {
		serverSocket->setDocumentRoot(dr);
	}

	virtual ~HTTPServer() {

	}
};

static HTTPServer *httpServer=NULL;

void signalHandler(int sig) {
	if ((sig == SIGINT)		||
		(sig == SIGSTOP)	||
		(sig == SIGTERM)	||
		(sig == SIGQUIT)) {
		if (httpServer) httpServer->stop();
	}
}

void printUsage(char *p) {
	cout << "usage: "<<p<<" [-a bind_addr] [-p bind_port]"<< endl;
	cout << "-a bind_addr: the IP address to bind the server to. Default 127.0.0.1"<< endl;
	cout << "-p bind_port: the server port number. Default 80"<< endl;
	cout << "-r dir: document root directory. Default $PWD."<< endl;
	exit(-1);
}

int main(int argc, char **argv) {
	int opt;
	string bindAddr="127.0.0.1";
	unsigned short bindPort=80;
	char cwd[BUFSIZ];
	string rootDir = getcwd(cwd,BUFSIZ);

	while ((opt = getopt(argc, argv, "a:p:r:h")) != -1) {
		switch (opt) {
		case 'a':
			bindAddr = optarg;
			break;
		case 'p':
			bindPort = atoi(optarg);
			break;
		case 'r':
			if (optarg) {
				if (optarg[0]=='/') rootDir = optarg;
				else rootDir = rootDir + "/" + optarg;
			}
			break;
		case 'h':
		default:
	    	printUsage(argv[0]);
	    break;
		}
	}

	if (rootDir.empty()		||
		bindAddr.empty()	||
		(bindPort == 0)) {
    	printUsage(argv[0]);
	} else {
		sigset_t sigset;

		if (rootDir[rootDir.length()-1]!='/') rootDir = rootDir + '/';

		sigemptyset(&sigset);
		sigaddset(&sigset,SIGCHLD);
		sigprocmask(SIG_BLOCK,&sigset,NULL); // needed by CGIControlThread.

    	httpServer = new HTTPServer(rootDir, 15);

    	signal(SIGINT,signalHandler);
    	signal(SIGTERM,signalHandler);
    	signal(SIGPIPE, SIG_IGN);
    	signal(SIGHUP,signalHandler);
    	signal(SIGSTOP,signalHandler);
    	signal(SIGQUIT,signalHandler);

		if (!httpServer->start(bindAddr,bindPort))
			perror("Could not start the server");
		delete httpServer;
	}
	exit(0);
}
