/*
 * SocketStream.h
 *
 *  Created on: 10 de jul de 2017
 *      Author: Benutzer
 */

#ifndef SRC_SOCKETSTREAM_H_
#define SRC_SOCKETSTREAM_H_

#include <iostream>
#include <memory>
#include "ClientSocket.h"

namespace socks {

class SocketStreamBuf : public std::streambuf {
public:
	SocketStreamBuf(std::unique_ptr<ClientSocket> clientSocket);
	virtual ~SocketStreamBuf();
protected:
    int_type underflow() override;
    int_type overflow(int_type __c  = traits_type::eof()) override;
    int sync() override;
private:
    static constexpr size_t buffSize = 4096;
    std::unique_ptr<char []> inp, outp;
    std::unique_ptr<ClientSocket> clientSocket;

    int_type transmit();
};

class SocketStream : public std::iostream {
public:
	SocketStream(std::unique_ptr<ClientSocket> clientSocket);
	SocketStream(SocketStream &&);
private:
	std::unique_ptr<SocketStreamBuf> socketStreamBuf;
};

} /* namespace socks */

#endif /* SRC_SOCKETSTREAM_H_ */
