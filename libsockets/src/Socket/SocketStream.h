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

#ifndef SRC_SOCKET_SOCKETSTREAM_H_
#define SRC_SOCKET_SOCKETSTREAM_H_

#include <iostream>
#include <memory>

#include "Socket/ClientSocket.h"

namespace socks {

class SocketStreamBuf : public std::streambuf {
public:
	SocketStreamBuf(ClientSocket &clientSocket);
	virtual ~SocketStreamBuf();
	ClientSocket &getClientSocket();
protected:
    int_type underflow() override;
    int_type overflow(int_type __c  = traits_type::eof()) override;
    int sync() override;
private:
    static constexpr size_t buffSize = 4096;
    std::unique_ptr<char []> inp, outp;
    ClientSocket &clientSocket;

    int_type transmit();
};

class SocketStream : public std::iostream {
public:
	SocketStream(std::unique_ptr<ClientSocket> clientSocket);
	SocketStream();
	SocketStream(SocketStream &&);
	SocketStream(ClientSocket &);
	int connectTo(const std::string &host, const std::string &port);
	ClientSocket &getClientSocket();
private:
	std::unique_ptr<ClientSocket> clientSocket;
	std::unique_ptr<SocketStreamBuf> socketStreamBuf;
};

namespace factory {
	SocketStream makeSocketStream();
	SocketStream makeUDPSocketStream();
}

} /* namespace socks */

#endif /* SRC_SOCKET_SOCKETSTREAM_H_ */
