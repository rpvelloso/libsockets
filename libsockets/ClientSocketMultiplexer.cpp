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

#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include "AbstractMultiplexedClientSocket.h"
#include "ClientSocketMultiplexer.h"

static char INTR_WAIT = 0x00;
static char CNCL_WAIT = 0x01;

#ifdef WIN32

int socketpair(int domain, int rawtype, int protocol, int socks[2])
{
    union {
       struct sockaddr_in inaddr;
       struct sockaddr addr;
    } a;
    int listener;
    int e;
    int addrlen = sizeof(a.inaddr);
    int reuse = 1;

    if (socks == 0) {
      WSASetLastError(WSAEINVAL);
      return SOCKET_ERROR;
    }

    listener = socket(domain, rawtype, protocol);
    if (listener == (int)INVALID_SOCKET)
        return SOCKET_ERROR;

    memset(&a, 0, sizeof(a));
    a.inaddr.sin_family = AF_INET;
    a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.inaddr.sin_port = 0;

    socks[0] = socks[1] = INVALID_SOCKET;
    do {
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
               (char*) &reuse, (int) sizeof(reuse)) == -1)
            break;
        if (bind(listener, &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR)
            break;

        memset(&a, 0, sizeof(a));
        if (getsockname(listener, &a.addr, &addrlen) == SOCKET_ERROR)
            break;
        // win32 getsockname may only set the port number, p=0.0005.
        // ( http://msdn.microsoft.com/library/ms738543.aspx ):
        a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        a.inaddr.sin_family = AF_INET;

        if (listen(listener, 1) == SOCKET_ERROR)
            break;

        socks[0] = socket(domain, rawtype, protocol);
        if (socks[0] == (int)INVALID_SOCKET)
            break;
        if (connect(socks[0], &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR)
            break;

        socks[1] = accept(listener, NULL, NULL);
        if (socks[1] == (int)INVALID_SOCKET)
            break;

        closesocket(listener);
        return 0;

    } while (0);

    e = WSAGetLastError();
    closesocket(listener);
    closesocket(socks[0]);
    closesocket(socks[1]);
    WSASetLastError(e);
    return SOCKET_ERROR;
}

#endif

ClientSocketMultiplexer::ClientSocketMultiplexer() : Object() {
	multiplexerState = MULTIPLEXER_IDLE;
	mutex = new Mutex();
#ifdef WIN32
	if ((socketpair(AF_INET, SOCK_STREAM, IPPROTO_TCP, controlSockets)) == -1) {
#else
	if ((socketpair(AF_LOCAL, SOCK_STREAM, 0, controlSockets)) == -1) {
#endif
		perror("socketpair()");
	} else {
		AbstractSocket::setNonBlocking(controlSockets[0],true);
		AbstractSocket::setNonBlocking(controlSockets[1],true);
	}
}

ClientSocketMultiplexer::~ClientSocketMultiplexer() {
	list<AbstractMultiplexedClientSocket *>::iterator i;

	cancelWait();

	while (multiplexerState != MULTIPLEXER_IDLE);

#ifdef WIN32
	shutdown(controlSockets[0],SD_BOTH);
	closesocket(controlSockets[0]);
	shutdown(controlSockets[1],SD_BOTH);
	closesocket(controlSockets[1]);
#else
	shutdown(controlSockets[0],SHUT_RDWR);
	close(controlSockets[0]);
	shutdown(controlSockets[1],SHUT_RDWR);
	close(controlSockets[1]);
#endif

	mutex->lock();
	for (i=sockets.begin();i!=sockets.end();i++) {
		delete (*i);
	}
	sockets.clear();
	mutex->unlock();
	delete mutex;
}

void ClientSocketMultiplexer::addSocket(AbstractMultiplexedClientSocket* s) {
	mutex->lock();
	s->setNonBlocking(true);
	sockets.push_back(s);
	s->setMultiplexer(this);
	mutex->unlock();
	interruptWaiting();
}

void ClientSocketMultiplexer::removeSocket(AbstractMultiplexedClientSocket* s) {
	mutex->lock();
	sockets.remove(s);
	mutex->unlock();
	delete s;
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
	return sockets.size();
}

MultiplexerState ClientSocketMultiplexer::getMultiplexerState() {
    return multiplexerState;
}

#define fd_is_valid(fd) ((fcntl(fd,F_GETFD) != -1) || (errno != EBADF))

void ClientSocketMultiplexer::waitForData() {
	fd_set rfds,wfds;
	int maxFd,fd,r=0;
	list<AbstractMultiplexedClientSocket *>::iterator i;
	char buffer[BUFSIZ];

	multiplexerState = MULTIPLEXER_WAITING;
	while (r>=0) {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(controlSockets[0],&rfds);
		maxFd=controlSockets[0];
		mutex->lock();
		for (i=sockets.begin();i!=sockets.end();i++) {
			fd = (*i)->getSocketFd();
			if (fd_is_valid(fd)) {
				FD_SET(fd,&rfds);
				if ((*i)->hasDataToSend()) FD_SET(fd,&wfds);
				if (fd>maxFd) maxFd = fd;
			} else {
				sockets.remove((*i));
				delete (*i);
			}
		}
		mutex->unlock();

		r = select(maxFd + 1, &rfds, &wfds, NULL, NULL);
		if (r>0) {
			list<AbstractMultiplexedClientSocket *> rs,ws;
			char c=0;

			mutex->lock();
			for (i=sockets.begin();i!=sockets.end();i++) {
				fd = ((AbstractMultiplexedClientSocket *)(*i))->getSocketFd();
				if (FD_ISSET(fd,&wfds)) ws.push_back(*i);
				if (FD_ISSET(fd,&rfds)) rs.push_back(*i);
			}
			mutex->unlock();

			for (i=rs.begin();i!=rs.end();i++) {
				(*i)->receiveData((void *)buffer,BUFSIZ);
				if ((*i)->getSocketStatus() != SOCKET_OPENED) {
					ws.remove((*i));
					removeSocket((*i));
				}
			}
			for (i=ws.begin();i!=ws.end();i++) {
				(*i)->transmitBuffer();
				if ((*i)->getSocketStatus() != SOCKET_OPENED) removeSocket((*i));
			}

			rs.clear();
			ws.clear();

			if (FD_ISSET(controlSockets[0],&rfds)) {
				while ((recv(controlSockets[0],&c,1,0) == 1) && !c);
				if (c) break;
			}
		} else {
			switch (errno) {
				case EINTR:
				case EBADF:
					r = 0;
					break;
				default:
					cout << "errno: " << errno << endl;
					perror("select()");
					break;
			}
		}
	}
	multiplexerState = MULTIPLEXER_IDLE;
}



