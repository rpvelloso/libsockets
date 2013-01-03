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

#include <sstream>
#include "EchoLogger.h"
#include "EchoClientSocket.h"

extern EchoLogger *logger;

EchoClientSocket::EchoClientSocket() : AbstractMultiplexedClientSocket() {
	outputBuffer = new stringstream;
}

EchoClientSocket::EchoClientSocket(int fd, sockaddr_in* sin) : AbstractMultiplexedClientSocket(fd,sin) {
	outputBuffer = new stringstream;
}

void EchoClientSocket::onSend(void *buf, size_t size) {
	if (!size) { // buf points to a string object
		logger->LOG("CLIENT: Enviando mensagem: %s\n",*((string *)buf)->c_str());
	} else {
		((char *)buf)[size]='\0';
		logger->LOG("CLIENT: Enviando mensagem: %s\n",(char *)buf);
	}
}

void EchoClientSocket::onReceive(void *buf, size_t size) {
	((char *)buf)[size]='\0';
	logger->LOG("CLIENT: Mensagem recebida: %s\n",(char *)buf);
	sendBufferedData(buf,size);
	commitBuffer();
	string msg = ((char *)buf);
}

void EchoClientSocket::onConnect() { logger->LOG("%s\n","CLIENT: Conectado."); };

void EchoClientSocket::beforeSend(void *buf, size_t &size) {
}

void EchoClientSocket::onDisconnect() {
	logger->LOG("%s\n", "CLIENT: Desconectado.");
}

