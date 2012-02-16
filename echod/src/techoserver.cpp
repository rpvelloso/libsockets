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
#include "techoserver.h"

tEchoServer::tEchoServer() : tObject() {
	server_socket = new tEchoServerSocket();
	log = new tEchoLog();
	log->Open();
	server_socket->SetLog(log);
}

tEchoServer::~tEchoServer() {
	list<tEchoThread *>::iterator i;

	for (i=threads.begin();i!=threads.end();i++) {
		(*i)->Stop();
	}
	threads.clear();
	delete server_socket;
	log->Close();
	delete log;
}

void tEchoServer::Run(const char *addr, unsigned short port) {
	tEchoClientSocket *client_socket;
	tEchoThread *client_thread;

	if (server_socket->Open(addr, port)) {
		cerr << "Error: could not listen on " << addr << ":" << port <<endl;
		perror("Open()");
	} else {
		while (server_socket->GetStatus() == tSocketListening) {
			client_socket = server_socket->Accept();
			if (client_socket) {
				client_socket->SetLog(log);
				client_thread = new tEchoThread(1, this, client_socket);
				threads.push_back(client_thread);
				client_thread->Start();
			} else {
				perror("Accept()");
			}
		}
	}
}

void tEchoServer::Stop() {
	server_socket->Close();
}

void tEchoServer::RemoveThread(tEchoThread *t) {
	threads.remove(t);
}

tEchoLog *tEchoServer::GetLog() {
	return log;
}
