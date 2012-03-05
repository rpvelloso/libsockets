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
#include "thttpserver.h"

tHTTPServer::tHTTPServer(string dr) : tObject() {
	serverSocket = new tHTTPServerSocket();
	log = new tHTTPLog();
	log->Open();
	serverSocket->setLog(log);
	documentRoot = dr;
	threadsMutex = new tMutex();
}

tHTTPServer::~tHTTPServer() {
	list<tHTTPThread *>::iterator i;

	threadsMutex->lock();
	for (i=threads.begin();i!=threads.end();i++) {
		(*i)->setSelfDestroy(0);
		delete (*i);
	}
	threads.clear();
	threadsMutex->unlock();
	delete serverSocket;
	log->Close();
	delete log;
	delete threadsMutex;
}

void tHTTPServer::run(const char *addr, unsigned short port) {
	tHTTPClientSocket *client_socket;
	tHTTPThread *client_thread;

	if (serverSocket->Open(addr, port)) {
		cerr << "Error: could not listen on " << addr << ":" << port <<endl;
		perror("Open()");
	} else {
		cout << "using document root \'" << documentRoot << "\'" << endl;
		while (serverSocket->getStatus() == tSocketListening) {
			client_socket = serverSocket->Accept();
			if (client_socket) {
				client_socket->setOwner(this);
				client_socket->setLog(log);
				client_thread = new tHTTPThread(1, this, client_socket);
				addThread(client_thread);
				client_thread->start();
			} else {
				perror("Accept()");
			}
		}
	}
}

void tHTTPServer::stop() {
	serverSocket->Close();
}

void tHTTPServer::removeThread(tHTTPThread *t) {
	threadsMutex->lock();
	threads.remove(t);
	threadsMutex->unlock();
}

void tHTTPServer::addThread(tHTTPThread *t) {
	threadsMutex->lock();
	threads.push_back(t);
	threadsMutex->unlock();
}

tHTTPLog *tHTTPServer::getLog() {
	return log;
}

string tHTTPServer::getDocumentRoot() {
	return documentRoot;
}

tHTTPServerSocket *tHTTPServer::getServerSocket() {
	return serverSocket;
}
