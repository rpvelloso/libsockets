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

#ifndef SRC_MULTIPLEXER_MULTIPLEXEDCLIENTSOCKET_H_
#define SRC_MULTIPLEXER_MULTIPLEXEDCLIENTSOCKET_H_

#include <iostream>
#include <functional>
#include <sstream>

#include "Multiplexer/MultiplexerImpl.h"
#include "Socket/ClientSocket.h"

namespace socks {

class MultiplexerImpl;

/*
 * Wrapper class. Adds multiplexing and buffering capabilities to a ClientSocket.
 * Signals the multiplexer if there is data to be sent and store client data/state.
 */

using MultiplexerCallback = std::function<void(std::istream &, std::ostream &)>;

class MultiplexedClientSocket {
public:
	MultiplexedClientSocket(
			std::unique_ptr<ClientSocket> impl,
			std::function<void()> interruptFunc,
			MultiplexerCallback readCallbackFunc,
			MultiplexerCallback connectCallbackFunc,
			MultiplexerCallback disconnectCallbackFunc,
			MultiplexerCallback writeCallbackFunc
			) :	readCallbackFunc(readCallbackFunc),
				connectCallbackFunc(connectCallbackFunc),
				disconnectCallbackFunc(disconnectCallbackFunc),
				writeCallbackFunc(writeCallbackFunc),
				impl(std::move(impl)),
				interruptFunc(interruptFunc) {

	};
	//virtual ~MultiplexedClientSocket() {};
	bool getHasOutput() { return outputBuffer.rdbuf()->in_avail() > 0; };
	void interrupt() { interruptFunc(); };
	std::stringstream &getOutputBuffer() {return outputBuffer;};
	std::stringstream &getInputBuffer() {return inputBuffer;};

	int receiveData(void *buf, size_t len) {return impl->receiveData(buf, len);};
	int sendData(const void *buf, size_t len) {return impl->sendData(buf, len);};
	size_t getSendBufferSize() const {return impl->getSendBufferSize();};
	size_t getReceiveBufferSize() const {return impl->getReceiveBufferSize();};
	SocketImpl &getImpl() {return impl->getImpl();};
	void readCallback() {readCallbackFunc(inputBuffer, outputBuffer);};
	void connectCallback() {connectCallbackFunc(inputBuffer, outputBuffer);};
	void disconnectCallback() {disconnectCallbackFunc(inputBuffer, outputBuffer);};
	void writeCallback() {writeCallbackFunc(inputBuffer, outputBuffer);};
private:
	MultiplexerCallback readCallbackFunc, connectCallbackFunc, disconnectCallbackFunc, writeCallbackFunc;
	std::unique_ptr<ClientSocket> impl;
	std::function<void()> interruptFunc;
	std::stringstream outputBuffer;
	std::stringstream inputBuffer;
};

}
#endif /* SRC_MULTIPLEXER_MULTIPLEXEDCLIENTSOCKET_H_ */
