/*
 * MultiplexedClientSocket.cpp
 *
 *  Created on: 29 de mai de 2017
 *      Author: rvelloso
 */

#include "MultiplexedClientSocket.h"

MultiplexedClientSocket::MultiplexedClientSocket(std::shared_ptr<SocketImpl> impl, std::function<void()> interruptFunc) : ClientSocket(impl), interruptFunc(interruptFunc) {
}

MultiplexedClientSocket::~MultiplexedClientSocket() {
}

bool MultiplexedClientSocket::getHasOutput() {
	return hasOutput;
}

void MultiplexedClientSocket::setHasOutput(bool hasOutput) {
	this->hasOutput = hasOutput;
}

void MultiplexedClientSocket::interrupt() {
	interruptFunc();
}
