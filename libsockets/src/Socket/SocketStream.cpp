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

#include "Socket/SocketStream.h"
#include "Socket/DatagramSocket.h"

namespace socks {

namespace factory {
	SocketStream makeSocketStream() {
		return SocketStream(std::make_unique<ClientSocket>(makeClientSocket()));
	};

	SocketStream makeUDPSocketStream() {
		return SocketStream(std::make_unique<ClientSocket>(makeUDPClientSocket()));
	};
}

SocketStreamBuf::SocketStreamBuf(ClientSocket &clientSocket) :
		std::streambuf(),
		inp(new char[buffSize]),
		outp(new char[buffSize]),
		clientSocket(clientSocket) {

		setp(outp.get(), outp.get() + buffSize - 1);
		setg(inp.get(), inp.get(), inp.get());
	};

SocketStreamBuf::~SocketStreamBuf() {};

ClientSocket &SocketStreamBuf::getClientSocket() {
	return clientSocket;
};

std::streambuf::int_type SocketStreamBuf::underflow() {
	auto received = clientSocket.receiveData(inp.get(), buffSize);

	if (received <= 0)
		return traits_type::eof();

	setg(inp.get(), inp.get(), inp.get() + received);
	return *gptr();
};

std::streambuf::int_type SocketStreamBuf::overflow(int_type __c) {
	if (__c != traits_type::eof()) {
		*pptr() = __c;
		pbump(1);
	}

	return transmit()==traits_type::eof()?traits_type::eof():__c;
};

int SocketStreamBuf::sync() {
	if (transmit() == traits_type::eof())
		return -1;
	return 0;
}

std::streambuf::int_type SocketStreamBuf::transmit() {
	auto len = pptr() - pbase();
	if (len > 0) {
		auto sent = clientSocket.sendData(outp.get(), len);
		// transmit
		if (sent > 0) {
			pbump(-sent);
			return sent;
		} // else ??? fail bit? eof ?
		return traits_type::eof();
	}
	return traits_type::eof();
}

SocketStream::SocketStream(std::unique_ptr<ClientSocket> clientSocket) :
		std::iostream(),
		clientSocket(std::move(clientSocket)),
		socketStreamBuf(new SocketStreamBuf(*(this->clientSocket))) {
	rdbuf(socketStreamBuf.get());
}

SocketStream::SocketStream() :
		std::iostream(),
		clientSocket(new ClientSocket()),
		socketStreamBuf(new SocketStreamBuf(*clientSocket)) {
	rdbuf(socketStreamBuf.get());
};

SocketStream::SocketStream(SocketStream &&rhs) : std::iostream(std::move(rhs)) {
	socketStreamBuf.swap(rhs.socketStreamBuf);
}

SocketStream::SocketStream(ClientSocket &clientSocket) :
		std::iostream(),
		socketStreamBuf(new SocketStreamBuf(clientSocket)) {
	rdbuf(socketStreamBuf.get());
}

int SocketStream::connectTo(const std::string &host, const std::string &port) {
	return socketStreamBuf->getClientSocket().connectTo(host, port);
};

ClientSocket& SocketStream::getClientSocket() {
	return *clientSocket;
}

} /* namespace socks */

