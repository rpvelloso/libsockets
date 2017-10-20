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

#ifndef SRC_SOCKET_BUFFEREDCLIENTSOCKETINTERFACE_H_
#define SRC_SOCKET_BUFFEREDCLIENTSOCKETINTERFACE_H_

#include <sstream>

#include "Socket/SocketImpl.h"

namespace socks {

/*
 * Wrapper class. Adds multiplexing and buffering capabilities to a ClientSocket.
 */

class BufferedClientSocketInterface {
public:
	BufferedClientSocketInterface() {};
	virtual ~BufferedClientSocketInterface() {};
	virtual bool getHasOutput() = 0;
	virtual std::stringstream &getOutputBuffer() = 0;
	virtual std::stringstream &getInputBuffer() = 0;

	virtual int receiveData(void *buf, size_t len) = 0;
	virtual int sendData(const void *buf, size_t len) = 0;
	virtual size_t getSendBufferSize() const = 0;
	virtual size_t getReceiveBufferSize() const = 0;
	virtual SocketImpl &getImpl() = 0;
	virtual void readCallback() = 0;
	virtual void connectCallback() = 0;
	virtual void disconnectCallback() = 0;
	virtual void writeCallback() = 0;
	virtual int setNonBlockingIO(bool status) = 0;
};

}
#endif /* SRC_SOCKET_BUFFEREDCLIENTSOCKETINTERFACE_H_ */
