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
		while ((pid = wait(&ret)) > 0) {
			HTTPClientSocket *s = removePID(pid);
			if (s) s->onCGIEnd();
		}
	}
}

void CGIControlThread::addPID(pid_t p, HTTPClientSocket* s) {
	mutex->lock();
	socketmap[p]=s;
	mutex->unlock();
}

HTTPClientSocket* CGIControlThread::removePID(pid_t p) {
	HTTPClientSocket *s;

	mutex->lock();
	s = socketmap[p];
	socketmap.erase(p);
	mutex->unlock();

	return s;
}
