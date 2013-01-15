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
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "AbstractMultiplexedClientSocket.h"
#include "ClientSocketMultiplexer.h"

static char INTR_WAIT = 0x00;
static char CNCL_WAIT = 0x01;

ClientSocketMultiplexer::ClientSocketMultiplexer() : Object() {
	multiplexerState = MULTIPLEXER_IDLE;
	mutex = new Mutex();
	if ((socketpair(AF_LOCAL, SOCK_STREAM, 0, controlSockets)) == -1) {
		perror("socketpair()");
	} else {
		AbstractSocket::setNonBlocking(controlSockets[0],true);
		AbstractSocket::setCloseOnExec(controlSockets[0]);
		AbstractSocket::setCloseOnExec(controlSockets[1]);
	}
}

ClientSocketMultiplexer::~ClientSocketMultiplexer() {
	map<int, AbstractMultiplexedClientSocket *>::iterator mi;

	cancelWait();

	while (multiplexerState != MULTIPLEXER_IDLE);

	shutdown(controlSockets[0],SHUT_RDWR);
	close(controlSockets[0]);
	shutdown(controlSockets[1],SHUT_RDWR);
	close(controlSockets[1]);

	updateSockets();
	for (mi=sockets.begin();mi!=sockets.end();mi++)
		delete (*mi).second;

	sockets.clear();

	delete mutex;
}

void ClientSocketMultiplexer::addSocket(AbstractMultiplexedClientSocket* s) {
	mutex->lock();
	insertedSockets.insert(s);
	mutex->unlock();
	interruptWaiting();
}

void ClientSocketMultiplexer::removeSocket(AbstractMultiplexedClientSocket* s) {
	mutex->lock();
	removedSockets.insert(s);
	mutex->unlock();
	interruptWaiting();
}

void ClientSocketMultiplexer::interruptWaiting() {
	if (multiplexerState != MULTIPLEXER_IDLE)
		send(controlSockets[1],&INTR_WAIT,1,0);
}

void ClientSocketMultiplexer::cancelWait() {
	if (multiplexerState != MULTIPLEXER_IDLE)
		send(controlSockets[1],&CNCL_WAIT,1,0);
}

size_t ClientSocketMultiplexer::getSocketCount() {
	size_t r;

	mutex->lock();
	r = sockets.size() + insertedSockets.size() - removedSockets.size();
	mutex->unlock();
	return r;
}

MultiplexerState ClientSocketMultiplexer::getMultiplexerState() {
    return multiplexerState;
}

#define fd_is_valid(fd) ((fcntl(fd,F_GETFL) != -1) || (errno != EBADF))
#define READY_TO_READ(x) (x.revents & POLLIN)
#define READY_TO_WRITE(x) (x.revents & POLLOUT)
#define POLL_READ(x) x.events |= POLLIN
#define POLL_WRITE(x) x.events |= POLLOUT

void ClientSocketMultiplexer::waitForData() {
	set<AbstractMultiplexedClientSocket *>::iterator i;
	map<int, AbstractMultiplexedClientSocket *>::iterator mi;
	AbstractMultiplexedClientSocket *s;
	struct pollfd *fds=NULL;
	int nfds,r=0;

	multiplexerState = MULTIPLEXER_WAITING;
	while (r>=0) {
		updateSockets();

		free(fds);
		if (!(fds = (struct pollfd *)calloc(sockets.size()+1,sizeof(struct pollfd)))) {
			perror("malloc()");
			break;
		}
		fds[0].fd = controlSockets[0];
		POLL_READ(fds[0]);
		nfds=1;

		for (mi=sockets.begin();mi!=sockets.end();++mi) {
			fds[nfds].fd = (*mi).first;
			s = (*mi).second;
			if (s->hasDataToSend()) POLL_WRITE(fds[nfds]);
			POLL_READ(fds[nfds]);
			nfds++;
		}

		r = poll(fds,nfds,-1);
		if (r>0) {
			set<AbstractMultiplexedClientSocket *> readSet,writeSet;
			char c=0;

			for (int j=1;j<nfds;j++) {
				if (READY_TO_WRITE(fds[j])) writeSet.insert(sockets[fds[j].fd]);
				if (READY_TO_READ(fds[j])) readSet.insert(sockets[fds[j].fd]);
			}

			for (i=readSet.begin();i!=readSet.end();++i) {
				(*i)->receiveData((void *)buffer,BUFSIZ);
				if ((*i)->getSocketStatus() != SOCKET_OPENED) {
					writeSet.erase((*i));
					removeSocket((*i));
				}
			}
			for (i=writeSet.begin();i!=writeSet.end();++i) {
				(*i)->transmitBuffer();
				if ((*i)->getSocketStatus() != SOCKET_OPENED) removeSocket((*i));
			}

			readSet.clear();
			writeSet.clear();

			if (READY_TO_READ(fds[0])) {
				while ((recv(controlSockets[0],&c,1,0) == 1) && !c);
				if (c) break;
			}
		} else {
			perror("poll()");
			r = 0;
		}
	}
	free(fds);
	multiplexerState = MULTIPLEXER_IDLE;
}

void ClientSocketMultiplexer::updateSockets() {
	map<int, AbstractMultiplexedClientSocket *>::iterator mi;
	set<AbstractMultiplexedClientSocket *>::iterator i;
	AbstractMultiplexedClientSocket *s;
	int fd;

	mutex->lock();

	// remove bad fd's and closed sockets
	for (mi=sockets.begin();mi!=sockets.end();++mi) {
		fd = (*mi).first;
		s = (*mi).second;
		if (!fd_is_valid(fd) || s->getSocketStatus() != SOCKET_OPENED)
			removedSockets.insert(s);
	}

	// removed sockets
	for (i=removedSockets.begin();i!=removedSockets.end();++i) {
		fd = (*i)->getSocketFd();
		if (sockets.find(fd)!=sockets.end()) {
			sockets.erase(fd);
			delete (*i);
		}
	}
	removedSockets.clear();

	// inserted sockets
	for (i=insertedSockets.begin();i!=insertedSockets.end();++i) {
		fd = (*i)->getSocketFd();
		if (fd_is_valid(fd) && (*i)->getSocketStatus() == SOCKET_OPENED) {
			sockets[fd]=(*i);
			(*i)->setNonBlocking(true);
			(*i)->setMultiplexer(this);
		}
	}
	insertedSockets.clear();

	mutex->unlock();
}
