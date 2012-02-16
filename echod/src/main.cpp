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
#include "techoserver.h"

tEchoServer *srv;

#ifdef WIN32
	#define SIGQUIT SIGBREAK
	#define SIGSTOP SIGABRT
#endif

void signal_handler(int sig)
{
	if ((sig == SIGINT) ||
		(sig == SIGSTOP) ||
		(sig == SIGTERM) ||
		(sig == SIGQUIT)) {
		srv->Stop();
	}
}

void pusage(char *p)
{
	cout << "usage: "<<p<<" [-a bind_addr] [-p bind_port]"<<endl;
	cout << "-a bind_addr: the IP address to bind the server to. Default 127.0.0.1"<<endl;
	cout << "-p bind_port: the server port number. Default 21"<<endl;
	exit(-1);
}

int main(int argc, char *argv[])
{
	int opt;
	string bind_addr="127.0.0.1";
	unsigned short bind_port=7;

	while ((opt = getopt(argc, argv, "a:p:h")) != -1) {
		switch (opt) {
		case 'a':
			bind_addr = optarg;
			break;
		case 'p':
			bind_port = atoi(optarg);
			break;
		case 'h':
		default:
	    	pusage(argv[0]);
	    break;
		}
	}

	if ((bind_port == 0) || bind_addr.empty()) {
    	pusage(argv[0]);
	} else {

#ifdef WIN32
    	if (WinSocketStartup()) {
    		perror("WinSocketStartup()");
    		return -1;
    	}
#endif

    	srv = new tEchoServer();

    	signal(SIGINT,signal_handler);
    	signal(SIGTERM,signal_handler);
#ifndef WIN32
    	signal(SIGHUP,signal_handler);
#endif
    	signal(SIGSTOP,signal_handler);
    	signal(SIGQUIT,signal_handler);

    	srv->Run(bind_addr.c_str(),bind_port);
    	delete srv;

#ifdef WIN32
    	WSACleanup();
#endif
    }

    return EXIT_SUCCESS;
}
