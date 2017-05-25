/*
 * ClientSocket.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <memory>
#include "Socket.h"
#include "SocketImpl.h"

class ClientSocket : public Socket {
public:
	ClientSocket(SocketImpl *impl);
	virtual ~ClientSocket();
	int receiveData(void *buf, size_t len);
	int sendData(const void *buf, size_t len);
	int connectTo(const std::string &host, const std::string &port);
	void disconnect();
};

#endif /* CLIENTSOCKET_H_ */
