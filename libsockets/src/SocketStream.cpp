/*
 * SocketStream.cpp
 *
 *  Created on: 10 de jul de 2017
 *      Author: Benutzer
 */

#include "SocketStream.h"

namespace socks {

SocketStreamBuf::SocketStreamBuf(socks::ClientSocket *clientSocket) :
		std::streambuf(),
		inp(new char[buffSize]),
		outp(new char[buffSize]),
		clientSocket(clientSocket) {

		setp(outp.get(), outp.get() + buffSize - 1);
		setg(inp.get(), inp.get(), inp.get());
	};

SocketStreamBuf::~SocketStreamBuf() {};

std::streambuf::int_type SocketStreamBuf::underflow() {
	auto received = clientSocket->receiveData(inp.get(), buffSize);

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
		auto sent = clientSocket->sendData(outp.get(), len);
		// transmit
		if (sent > 0) {
			pbump(-sent);
			return sent;
		} // else ??? fail bit? eof ?
	}
	return traits_type::eof();
}

SocketStream::SocketStream(ClientSocket *clientSocket) :
		std::iostream(),
		socketStreamBuf(clientSocket) {
	rdbuf(&socketStreamBuf);
}

} /* namespace socks */
