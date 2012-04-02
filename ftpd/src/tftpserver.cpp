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
#include <fstream>
#include <errno.h>
#include "ftpreply.h"
#include "tftpserver.h"

tFTPServer::tFTPServer(int max, string loginfile) : tObject() {
	serverSocket = new tFTPServerSocket();
	log = new tFTPLog();
	log->Open();
	serverSocket->setLog(log);
	maxClients = max;
	totalClients = 0;
	totalClientsMutex = new tMutex();
	threadsMutex = new tMutex();
	loadLogins(loginfile);
}

tFTPServer::~tFTPServer() {
	list<tFTPThread *>::iterator i;
	list<tLogin *>::iterator j;

	threadsMutex->lock();
	i = threads.begin();
	for (i=threads.begin();i!=threads.end();i++) {
		(*i)->setSelfDestroy(0);
		delete (*i);
	}
	threads.clear();
	threadsMutex->unlock();

	j = users.begin();
	for (j=users.begin();j!=users.end();j++) {
		delete (*j);
	}
	users.clear();

	delete serverSocket;
	log->Close();
	delete log;
	delete totalClientsMutex;
	delete threadsMutex;
}

void tFTPServer::run(const char *addr, unsigned short port) {
	tFTPClientSocket *client_socket;
	tFTPThread *client_thread;
#ifndef WIN32
	char errstr[ERRSTR_LEN];
#endif

	if (serverSocket->Open(addr, port)) {
		log->log("error: can't listen on %s:%p - %s\n",addr,port,strerror_r(errno,errstr,ERRSTR_LEN));
	} else {
		log->log("Server limit: %d clients.\n",maxClients);
		while (serverSocket->getStatus() == tSocketListening) {
			client_socket = serverSocket->Accept();
			if (client_socket) {
				client_socket->setLog(log);
				if (getTotalClients() < maxClients) {
					setTotalClients(+1);
					client_socket->Send(R220);
					client_thread = new tFTPThread(1, this, client_socket);
					addThread(client_thread);
					client_thread->start();
				} else {
					log->log(client_socket,"server full.\n");
					client_socket->Send(R421);
					delete client_socket;
				}
			} else {
				log->log("error: accept error - %s\n",strerror_r(errno,errstr,ERRSTR_LEN));
			}
		}
	}
}

void tFTPServer::stop() {
	serverSocket->Close();
}

void tFTPServer::removeThread(tFTPThread *t) {
	threadsMutex->lock();
	threads.remove(t);
	threadsMutex->unlock();
}

void tFTPServer::addThread(tFTPThread *t) {
	threadsMutex->lock();
	threads.push_back(t);
	threadsMutex->unlock();
}

tFTPLog *tFTPServer::getLog() {
	return log;
}

void tFTPServer::setMaxClients(int max) {
	maxClients = max;
}

int tFTPServer::getMaxClients() {
	return maxClients;
}

int tFTPServer::getTotalClients() {
	int r;

	totalClientsMutex->lock();
	r = totalClients;
	totalClientsMutex->unlock();
	return r;
}

void tFTPServer::setTotalClients(int inc) {
	totalClientsMutex->lock();
	totalClients += inc;
	totalClientsMutex->unlock();
}

tLogin *tFTPServer::authenticate(string u, string p) {
	list<tLogin *>::iterator i;

	i = users.begin();
	for (i=users.begin();i!=users.end();i++) {
		if ((*i)->username == u) {
			if ((u == "anonymous") || (*i)->password == p) return (*i);
			else return NULL;
		}
	}
	return NULL;
}

void tFTPServer::loadLogins(string f) {
	fstream logins;
	char buf[256];
	string t,line;
	tLogin *l;
	int i;

	logins.open(f.c_str(),fstream::in);
	while (!logins.eof() && !logins.fail()) {
		memset(buf,0,256);
		logins.getline(buf,256);
		if (buf[0] != '#') {
			line = buf;
			t = stringTok(line,":");
			i = 0;
			while ((!t.empty()) && (i<=2)) {
				switch (i) {
				case 0:
					l = new tLogin;
					l->username = t;
					i++;
					break;
				case 1:
					l->password = t;
					i++;
					break;
				case 2:
					l->homedir = t;
					users.push_back(l);
					l = NULL;
					i++;
					break;
				}
				t = stringTok(line,":");
			}
		}
	}
}
