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
#include "tftpserver.h"

tFTPServer *srv;

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

void pusage(char *p)
{
	cout << "usage: "<<p<<" -l login_file [-a bind_addr] [-p bind_port] [-m max_clients] [-r dir]"<<endl;
	cout << "-l login_file: login file with user information for authentication."<<endl;
	cout << "-a bind_addr: the IP address to bind the server to. Default 127.0.0.1"<<endl;
	cout << "-p bind_port: the server port number. Default 21"<<endl;
	cout << "-m max_clients: maximum simultaneous clients. Default 100."<<endl;
	cout << "-r dir: chroot directory. Default \"/\"."<<endl;
	exit(-1);
}

int main(int argc, char *argv[])
{
	int opt,max=100;
	string bind_addr="127.0.0.1",login_file="",rdir="/";
	unsigned short bind_port=21;

	while ((opt = getopt(argc, argv, "a:p:l:m:r:h")) != -1) {
		switch (opt) {
		case 'a':
			bind_addr = optarg;
			break;
		case 'p':
			bind_port = atoi(optarg);
			break;
		case 'l':
			login_file = optarg;
			break;
		case 'm':
			max = atoi(optarg);
			break;
		case 'r':
			rdir = optarg;
			if (rdir[rdir.length()-1]!='/') rdir = rdir + '/';
			break;
		case 'h':
		default:
	    	pusage(argv[0]);
	    	break;
		}
	}

	if ((rdir.empty())      || (bind_port == 0)     ||
		(bind_addr.empty()) || (login_file.empty()) || (max == 0)) {
    	pusage(argv[0]);
	} else {

#ifdef WIN32
    	if (WinSocketStartup()) {
    		perror("WinSocketStartup()");
    		return -1;
    	}
#endif

#ifndef WIN32
    	if (rdir != "/") chroot(rdir.c_str());
#endif

    	srv = new tFTPServer(max,login_file);

    	signal(SIGINT,signalHandler);
    	signal(SIGTERM,signalHandler);
#ifndef WIN32
    	signal(SIGPIPE, SIG_IGN);
    	signal(SIGHUP,signalHandler);
#endif
    	signal(SIGSTOP,signalHandler);
    	signal(SIGQUIT,signalHandler);

    	srv->run(bind_addr.c_str(),bind_port);
    	delete srv;

#ifdef WIN32
    	WSACleanup();
#endif
    }

    return EXIT_SUCCESS;
}
