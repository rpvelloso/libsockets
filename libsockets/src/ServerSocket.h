/*
 * ServerSocket.h
 *
 *  Created on: 25 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <string>
#include "Socket.h"
#include "SocketImpl.h"

namespace socks {

class ServerSocket : public Socket {
public:
	ServerSocket(ServerSocket &&) = default;
	ServerSocket(SocketImpl *impl);
	ServerSocket();
	virtual ~ServerSocket();
	int listenForConnections(const std::string &bindAddr, const std::string &port);
	ClientSocket acceptConnection();
	void disconnect();
};

}
#endif /* SERVERSOCKET_H_ */
