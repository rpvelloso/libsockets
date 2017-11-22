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
#include "Socket/SocketStream.h"

namespace socks {

template<class ClientContext>
class Context {
public:
	Context(ClientSocket &clientSocket) :
		localAddress(clientSocket.getLocalAddress()),
		remoteAddress(clientSocket.getRemoteAddress()) {
	}

	ClientContext &getContext() {
		return context;
	}

	SocketAddress &getLocalAddress() {
		return localAddress;
	}

	SocketAddress &getRemoteAddress() {
		return remoteAddress;
	}
private:
	ClientContext context;
	SocketAddress localAddress, remoteAddress;
};

template<class ClientContext>
using ClientCallback = std::function<void(Context<ClientContext> &, std::istream &, std::ostream &)>;

/*
 * Wrapper class. Adds multiplexing and buffering capabilities to a ClientSocket.
 */

template<class ClientContext>
class BufferedClientSocket : public BufferedClientSocketInterface {
public:
	BufferedClientSocket(
		std::unique_ptr<ClientSocket> impl,
		ClientCallback<ClientContext> readCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) :
		impl(std::move(impl)),
		readCB(readCallback),
		connectCB(connectCallback),
		disconnectCB(disconnectCallback),
		writeCB(writeCallback),
		outputSocketBuffer(*(this->impl)),
		outp(&outputSocketBuffer),
		clientData(*(this->impl)) {};
	virtual ~BufferedClientSocket() {};
	bool getHasOutput() override {return outputBuffer.rdbuf()->in_avail() > 0;};
	std::stringstream &getOutputBuffer() override {return outputBuffer;};
	std::stringstream &getInputBuffer() override {return inputBuffer;};

	int receiveData(void *buf, size_t len) override {return impl->receiveData(buf, len);};
	int sendData(const void *buf, size_t len) override {return impl->sendData(buf, len);};
	size_t getSendBufferSize() const override {return impl->getSendBufferSize();};
	size_t getReceiveBufferSize() const override {return impl->getReceiveBufferSize();};
	int setNonBlockingIO(bool status) override {
		if (!status)
			outp = &outputSocketBuffer; // unbuffered output for blocking I/O
		else
			outp = &outputBuffer;

		return impl->setNonBlockingIO(status);
	};
	SocketImpl &getImpl() override {return impl->getImpl();};
	ClientSocket &getSocket() override {return *impl;};
	void readCallback() override {readCB(clientData, inputBuffer, *outp);};
	void connectCallback() override {connectCB(clientData, inputBuffer, *outp);};
	void disconnectCallback() override {disconnectCB(clientData, inputBuffer, *outp);};
	void writeCallback() override {writeCB(clientData, inputBuffer, *outp);};
private:
	std::unique_ptr<ClientSocket> impl;
	ClientCallback<ClientContext> readCB, connectCB, disconnectCB, writeCB;
	std::stringstream outputBuffer;
	std::stringstream inputBuffer;
	SocketStream outputSocketBuffer;
	std::ostream *outp;
	Context<ClientContext> clientData;
};

}
#endif /* SRC_SOCKET_BUFFEREDCLIENTSOCKET_H_ */
