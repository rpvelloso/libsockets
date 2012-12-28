/*
 * CGIControlThread.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: roberto
 */

#include <csignal>
#include "HTTPClientSocket.h"
#include "CGIControlThread.h"

CGIControlThread::CGIControlThread() : AbstractThread() {
	mutex = new Mutex();
}

CGIControlThread::~CGIControlThread() {
	delete mutex;
	socketmap.clear();
}

void CGIControlThread::onStart() {
}

void CGIControlThread::onStop() {
}

void CGIControlThread::execute() {
	pid_t pid;
	int ret,sig;
	sigset_t waitset;

	sigemptyset(&waitset);
	sigaddset(&waitset,SIGCHLD); // need to block this signal in the main thread!!!

	while (true) {
		sigwait(&waitset,&sig);
		pid = wait(&ret);
		if (pid > 0) {
			HTTPClientSocket *s = removeSocket(pid);
			if (s) processCGIOutput(s);
		}
	}
}

void CGIControlThread::addPID(pid_t p, HTTPClientSocket* s) {
	mutex->lock();
	socketmap[p]=s;
	mutex->unlock();
}

HTTPClientSocket* CGIControlThread::removeSocket(pid_t p) {
	HTTPClientSocket *s;

	mutex->lock();
	s = socketmap[p];
	socketmap.erase(p);
	mutex->unlock();

	return s;
}

void CGIControlThread::processCGIOutput(HTTPClientSocket *clientSocket) {
	if (clientSocket->CGIInput.is_open()) clientSocket->CGIInput.tmp_close();
	clientSocket->CGIOutput.seekg(0);
	if (clientSocket->CGIOutput.rdbuf()->in_avail() > 0) {
		string l,L;

		getline(clientSocket->CGIOutput,l);
		L = l.substr(0,11);
		upperCase(L);
		if (L.substr(0,8) == "STATUS: ") {
			clientSocket->sendBufferedData(clientSocket->httpVersion + L.substr(7,4) + CRLF);
			clientSocket->sendBufferedData("Connection: " + clientSocket->connection + CRLF);
		} else if (L.substr(0,5) != "HTTP/") {
			clientSocket->sendBufferedData(clientSocket->httpVersion + " 200 OK" + CRLF);
			clientSocket->sendBufferedData("Connection: " + clientSocket->connection + CRLF);
			clientSocket->CGIOutput.seekg(0);
		} else clientSocket->sendBufferedData(l);
	} else {
		clientSocket->CGIOutput.close();
		clientSocket->requestState = HTTP_REQUEST_ENDED;
	}
	clientSocket->CGIPid = -1;
	clientSocket->commitBuffer();
}
