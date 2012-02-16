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
	server_socket = new tFTPServerSocket();
	log = new tFTPLog();
	log->Open();
	server_socket->SetLog(log);
	max_clients = max;
	total_clients = 0;
	total_clients_mutex = new tMutex();
	threads_mutex = new tMutex();
	LoadLogins(loginfile);
}

tFTPServer::~tFTPServer() {
	list<tFTPThread *>::iterator i;
	list<tLogin *>::iterator j;

	threads_mutex->Lock();
	i = threads.begin();
	for (i=threads.begin();i!=threads.end();i++) {
		(*i)->SetSelfDestroy(0);
		delete (*i);
	}
	threads.clear();
	threads_mutex->Unlock();

	j = users.begin();
	for (j=users.begin();j!=users.end();j++) {
		delete (*j);
	}
	users.clear();

	delete server_socket;
	log->Close();
	delete log;
	delete total_clients_mutex;
	delete threads_mutex;
}

void tFTPServer::Run(const char *addr, unsigned short port) {
	tFTPClientSocket *client_socket;
	tFTPThread *client_thread;
#ifndef WIN32
	char errstr[ERRSTR_LEN];
#endif

	if (server_socket->Open(addr, port)) {
		log->Log("error: can't listen on %s:%p - %s\n",addr,port,strerror_r(errno,errstr,ERRSTR_LEN));
	} else {
		log->Log("Server limit: %d clients.\n",max_clients);
		while (server_socket->GetStatus() == tSocketListening) {
			client_socket = server_socket->Accept();
			if (client_socket) {
				client_socket->SetLog(log);
				if (GetTotalClients() < max_clients) {
					SetTotalClients(+1);
					client_socket->Send(R220);
					client_thread = new tFTPThread(1, this, client_socket);
					AddThread(client_thread);
					client_thread->Start();
				} else {
					log->Log(client_socket,"server full.\n");
					client_socket->Send(R421);
					delete client_socket;
				}
			} else {
				log->Log("error: accept error - %s\n",strerror_r(errno,errstr,ERRSTR_LEN));
			}
		}
	}
}

void tFTPServer::Stop() {
	server_socket->Close();
}

void tFTPServer::RemoveThread(tFTPThread *t) {
	threads_mutex->Lock();
	threads.remove(t);
	threads_mutex->Unlock();
}

void tFTPServer::AddThread(tFTPThread *t) {
	threads_mutex->Lock();
	threads.push_back(t);
	threads_mutex->Unlock();
}

tFTPLog *tFTPServer::GetLog() {
	return log;
}

void tFTPServer::SetMaxClients(int max) {
	max_clients = max;
}

int tFTPServer::GetMaxClients() {
	return max_clients;
}

int tFTPServer::GetTotalClients() {
	int r;

	total_clients_mutex->Lock();
	r = total_clients;
	total_clients_mutex->Unlock();
	return r;
}

void tFTPServer::SetTotalClients(int inc) {
	total_clients_mutex->Lock();
	total_clients += inc;
	total_clients_mutex->Unlock();
}

tLogin *tFTPServer::Authenticate(string u, string p) {
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

void tFTPServer::LoadLogins(string f) {
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
			t = stringtok(&line,":");
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
				t = stringtok(&line,":");
			}
		}
	}
}
