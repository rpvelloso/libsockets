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
#include "Socket/BufferedClientSocketInterface.h"

namespace socks {

template<class ClientContext>
using ClientCallback = std::function<void(ClientContext &, std::istream &, std::ostream &)>;

/*
 * Wrapper class. Adds multiplexing and buffering capabilities to a ClientSocket.
 */

template<class ClientContext>
class BufferedClientSocket : public BufferedClientSocketInterface {
public:
	BufferedClientSocket(
		std::unique_ptr<ClientSocket> impl,
		ClientCallback<ClientContext> readCallback = [](ClientContext &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> connectCallback = [](ClientContext &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> disconnectCallback = [](ClientContext &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> writeCallback = [](ClientContext &ctx, std::istream &inp, std::ostream &outp){}) :
		impl(std::move(impl)),
		readCB(readCallback),
		connectCB(connectCallback),
		disconnectCB(disconnectCallback),
		writeCB(writeCallback) {};
	virtual ~BufferedClientSocket() {};
	bool getHasOutput() override {return outputBuffer.rdbuf()->in_avail() > 0;};
	std::stringstream &getOutputBuffer() override {return outputBuffer;};
	std::stringstream &getInputBuffer() override {return inputBuffer;};

	int receiveData(void *buf, size_t len) override {return impl->receiveData(buf, len);};
	int sendData(const void *buf, size_t len) override {return impl->sendData(buf, len);};
	size_t getSendBufferSize() const override {return impl->getSendBufferSize();};
	size_t getReceiveBufferSize() const override {return impl->getReceiveBufferSize();};
	int setNonBlockingIO(bool status) override {return impl->setNonBlockingIO(status);};
	SocketImpl &getImpl() override {return impl->getImpl();};
	void readCallback() override {readCB(clientData, inputBuffer, outputBuffer);};
	void connectCallback() override {connectCB(clientData, inputBuffer, outputBuffer);};
	void disconnectCallback() override {disconnectCB(clientData, inputBuffer, outputBuffer);};
	void writeCallback() override {writeCB(clientData, inputBuffer, outputBuffer);};
private:
	std::unique_ptr<ClientSocket> impl;
	ClientCallback<ClientContext> readCB, connectCB, disconnectCB, writeCB;
	std::stringstream outputBuffer;
	std::stringstream inputBuffer;
	ClientContext clientData;
};

}
#endif /* SRC_SOCKET_BUFFEREDCLIENTSOCKET_H_ */
