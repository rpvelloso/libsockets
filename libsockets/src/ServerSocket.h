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
	ServerSocket(SocketImpl *impl);
	virtual ~ServerSocket();
	virtual int listenForConnections(const std::string &bindAddr, const std::string &port);
	virtual std::unique_ptr<ClientSocket> acceptConnection();
	virtual void disconnect();
};

}
#endif /* SERVERSOCKET_H_ */
