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
#include "ClientSocketMultiplexer.h"
#include "AbstractMultiplexedClientSocket.h"

AbstractMultiplexedClientSocket::AbstractMultiplexedClientSocket() : AbstractClientSocket() {
	multiplexer = NULL;
	outputBuffer = NULL;
}

AbstractMultiplexedClientSocket::AbstractMultiplexedClientSocket(int fd, sockaddr_in *sin, SSL_CTX *ctx=NULL) : AbstractClientSocket(fd,sin,ctx) {
	multiplexer = NULL;
	outputBuffer = NULL;
}

AbstractMultiplexedClientSocket::~AbstractMultiplexedClientSocket() {
}

bool AbstractMultiplexedClientSocket::hasDataToSend() {
	return outputBuffer->rdbuf()->in_avail() > 0;
}

void AbstractMultiplexedClientSocket::sendBufferedData(void *buf, streamsize size) {
	outputBuffer->write((char *)buf,size);
}

void AbstractMultiplexedClientSocket::sendBufferedData(string buf) {
	(*outputBuffer) << buf;
}

void AbstractMultiplexedClientSocket::transmitBuffer() {
	char buf[BUFSIZ];
	int size;
	streamsize ssize;

	outputBuffer->readsome(buf,BUFSIZ);
	ssize = outputBuffer->gcount();
	if (ssize > 0) {
		size = sendData(buf,ssize);
		if (size != -1) {
			if (size != ssize) outputBuffer->seekg(size-ssize,ios_base::cur);
		} else outputBuffer->seekg(-ssize,ios_base::cur);
	}
}

void AbstractMultiplexedClientSocket::setMultiplexer(ClientSocketMultiplexer* m) {
	multiplexer = m;
}

void AbstractMultiplexedClientSocket::commitBuffer() {
	if (multiplexer && hasDataToSend()) multiplexer->interruptWaiting();
}
