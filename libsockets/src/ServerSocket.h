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

class ServerSocket : public Socket {
public:
	ServerSocket(std::shared_ptr<SocketImpl> impl);
	virtual ~ServerSocket();
	int listenForConnections(const std::string &bindAddr, const std::string &port);
	std::unique_ptr<ClientSocket> acceptConnection();
};

#endif /* SERVERSOCKET_H_ */
