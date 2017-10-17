/*
 * ThreadedConnectionPoolImpl.cpp
 *
 *  Created on: 17 de out de 2017
 *      Author: rvelloso
 */

#include <thread>
#include <sstream>

#include "Socket/ClientSocket.h"
#include "ConnectionPool/ThreadedConnectionPoolImpl.h"

namespace socks {

void threadFunction(MultiplexedClientSocket clientSocket) {
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
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) :
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
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {

	clientSocket->setNonBlockingIO(false);

	std::thread th(
			threadFunction,
			MultiplexedClientSocket(
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
