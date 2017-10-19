/*
    Copyright 2017 Roberto Panerai Velloso.
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

#include "Factory/SocketFactory.h"
#include "Socket/BufferedClientSocket.h"

namespace socks {

BufferedClientSocket::BufferedClientSocket(
		std::unique_ptr<ClientSocket> impl,
		ClientCallback readCallback,
		ClientCallback connectCallback,
		ClientCallback disconnectCallback,
		ClientCallback writeCallback
		) :	readCallbackFunc(readCallback),
			connectCallbackFunc(connectCallback),
			disconnectCallbackFunc(disconnectCallback),
			writeCallbackFunc(writeCallback),
			impl(std::move(impl)) {

	id = SocketFactory::createID();
};

bool BufferedClientSocket::getHasOutput() {
	return outputBuffer.rdbuf()->in_avail() > 0;
};

std::stringstream &BufferedClientSocket::getOutputBuffer() {
	return outputBuffer;
};

std::stringstream &BufferedClientSocket::getInputBuffer() {
	return inputBuffer;
};

int BufferedClientSocket::receiveData(void *buf, size_t len) {
	return impl->receiveData(buf, len);
};

int BufferedClientSocket::sendData(const void *buf, size_t len) {
	return impl->sendData(buf, len);
};

size_t BufferedClientSocket::getSendBufferSize() const {
	return impl->getSendBufferSize();
};

size_t BufferedClientSocket::getReceiveBufferSize() const {
	return impl->getReceiveBufferSize();
};

SocketImpl &BufferedClientSocket::getImpl() {
	return impl->getImpl();
};

int BufferedClientSocket::setNonBlockingIO(bool status) {
	return impl->setNonBlockingIO(status);
}

void BufferedClientSocket::readCallback() {
	readCallbackFunc(id, inputBuffer, outputBuffer);
};

void BufferedClientSocket::connectCallback() {
	connectCallbackFunc(id, inputBuffer, outputBuffer);
};

void BufferedClientSocket::disconnectCallback() {
	disconnectCallbackFunc(id, inputBuffer, outputBuffer);
};

void BufferedClientSocket::writeCallback() {
	writeCallbackFunc(id, inputBuffer, outputBuffer);
};

}

