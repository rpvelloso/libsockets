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

#ifndef SRC_SOCKET_BUFFEREDCLIENTSOCKET_H_
#define SRC_SOCKET_BUFFEREDCLIENTSOCKET_H_

#include <functional>
#include <sstream>

#include "Socket/ClientSocket.h"

namespace socks {

/*
 * Wrapper class. Adds multiplexing and buffering capabilities to a ClientSocket.
 */

class BufferedClientSocket {
public:
	BufferedClientSocket(
			std::unique_ptr<ClientSocket> impl,
			ClientCallback readCallback = defaultCallback,
			ClientCallback connectCallback = defaultCallback,
			ClientCallback disconnectCallback = defaultCallback,
			ClientCallback writeCallback = defaultCallback
			);
	bool getHasOutput();
	std::stringstream &getOutputBuffer();
	std::stringstream &getInputBuffer();

	int receiveData(void *buf, size_t len);
	int sendData(const void *buf, size_t len);
	size_t getSendBufferSize() const;
	size_t getReceiveBufferSize() const;
	SocketImpl &getImpl();
	void readCallback();
	void connectCallback();
	void disconnectCallback();
	void writeCallback();
	int setNonBlockingIO(bool status);
private:
	ClientCallback readCallbackFunc, connectCallbackFunc, disconnectCallbackFunc, writeCallbackFunc;
	std::unique_ptr<ClientSocket> impl;
	std::stringstream outputBuffer;
	std::stringstream inputBuffer;
	size_t id;
};

}
#endif /* SRC_SOCKET_BUFFEREDCLIENTSOCKET_H_ */
