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
#include <signal.h>
#include <unistd.h>
#include "thttpserver.h"

tHTTPServer *srv;

#ifdef WIN32
	#define SIGQUIT SIGBREAK
	#define SIGSTOP SIGABRT
#endif

void signalHandler(int sig)
{
	if ((sig == SIGINT) ||
		(sig == SIGSTOP) ||
		(sig == SIGTERM) ||
		(sig == SIGQUIT)) {
		srv->stop();
	}
}

void printUsage(char *p)
{
	cout << "usage: "<<p<<" [-a bind_addr] [-p bind_port]"<<endl;
	cout << "-a bind_addr: the IP address to bind the server to. Default 127.0.0.1"<<endl;
	cout << "-p bind_port: the server port number. Default 80"<<endl;
	cout << "-r dir: document root directory, must by an absolute path. Default \"/\"."<<endl;
	exit(-1);
}

int main(int argc, char *argv[])
{
	int opt;
	string bindAddr="127.0.0.1",rootDir="/";
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
    	if (WinSocketStartup()) {
    		perror("WinSocketStartup()");
    		return -1;
    	}
#endif

#ifndef WIN32
    	if (rootDir != "/") chroot(rootDir.c_str());
#endif

    	srv = new tHTTPServer(rootDir);

    	signal(SIGINT,signalHandler);
    	signal(SIGTERM,signalHandler);
#ifndef WIN32
    	signal(SIGHUP,signalHandler);
#endif
    	signal(SIGSTOP,signalHandler);
    	signal(SIGQUIT,signalHandler);

    	srv->run(bindAddr.c_str(),bindPort);
    	delete srv;

#ifdef WIN32
    	WSACleanup();
#endif
    }

    return EXIT_SUCCESS;
}
