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

#include <thread>
#include <sstream>

#include "Socket/ClientSocket.h"
#include "Socket/BufferedClientSocket.h"
#include "ConnectionPool/ThreadedConnectionPoolImpl.h"

namespace socks {

void threadFunction(BufferedClientSocket clientSocket) {
	auto recvBufSize = clientSocket.getReceiveBufferSize();
	char recvBuf[recvBufSize];
	int len;

	clientSocket.connectCallback();

	while (true) {
		auto &outputBuffer = clientSocket.getOutputBuffer();
		auto &inputBuffer = clientSocket.getInputBuffer();
		while (clientSocket.getHasOutput()) {
			auto sndBufSize = outputBuffer.rdbuf()->in_avail();
			char sndBuf[sndBufSize];

			outputBuffer.readsome(sndBuf, sndBufSize);
			clientSocket.sendData(sndBuf, outputBuffer.gcount());
		}
		outputBuffer.clear();
		outputBuffer.str(std::string());
		clientSocket.writeCallback();

		if ((len = clientSocket.receiveData(recvBuf, recvBufSize)) > 0) {
			inputBuffer.write(recvBuf, len);
			clientSocket.readCallback();
		} else
			break;
	}

	clientSocket.disconnectCallback();
};

ThreadedConnectionPoolImpl::ThreadedConnectionPoolImpl(
		ClientCallback readCallback,
		ClientCallback connectCallback,
		ClientCallback disconnectCallback,
		ClientCallback writeCallback) :
				ConnectionPoolImpl(),
				readCB(readCallback),
				connectCB(connectCallback),
				disconnectCB(disconnectCallback),
				writeCB(writeCallback) {
}

ThreadedConnectionPoolImpl::~ThreadedConnectionPoolImpl() {
}

void ThreadedConnectionPoolImpl::addClientSocket(
		std::unique_ptr<ClientSocket> clientSocket,
		ClientCallback readCallback,
		ClientCallback connectCallback,
		ClientCallback disconnectCallback,
		ClientCallback writeCallback) {

	clientSocket->setNonBlockingIO(false);

	std::thread th(
			threadFunction,
			BufferedClientSocket(
				std::move(clientSocket),
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));

	th.detach();
}

void ThreadedConnectionPoolImpl::addClientSocket(
		std::unique_ptr<ClientSocket> clientSocket) {

	addClientSocket(
			std::move(clientSocket),
			readCB,
			connectCB,
			disconnectCB,
			writeCB);
}

} /* namespace socks */