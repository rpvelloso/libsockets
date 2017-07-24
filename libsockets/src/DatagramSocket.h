/*
 * DatagramSocket.h
 *
 *  Created on: 21 de jul de 2017
 *      Author: rvelloso
 */

#ifndef SRC_DATAGRAMSOCKET_H_
#define SRC_DATAGRAMSOCKET_H_

#include "Socket.h"
#include "SocketImpl.h"
#include "ClientSocket.h"

namespace socks {

class DatagramSocket: public Socket {
public:
	DatagramSocket(DatagramSocket &&) = default;
	DatagramSocket(SocketImpl *impl);
	virtual ~DatagramSocket();
	std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len);
	int sendTo(const SocketAddress &addr, const void *buf, size_t len);
	int bindSocket(const std::string &bindAddr, const std::string &port);
	ClientSocket makeClientSocket(const SocketAddress &addr);
	ClientSocket makeClientSocket(const std::string &host, const std::string &port);
};

} /* namespace socks */

#endif /* SRC_DATAGRAMSOCKET_H_ */
