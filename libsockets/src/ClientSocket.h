/*
 * ClientSocket.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <memory>
#include "SocketImpl.h"
#include "Socket.h"

class ClientSocket : public Socket {
public:
	ClientSocket(std::shared_ptr<SocketImpl> impl);
	virtual ~ClientSocket();
	virtual int receiveData(void *buf, size_t len);
	virtual int sendData(const void *buf, size_t len);
	virtual int connectTo(const std::string &host, const std::string &port);
	virtual void disconnect();
	size_t getSendBufferSize() const;
	size_t getReceiveBufferSize() const;

private:
	size_t sendBufferSize = 0;
	size_t receiveBufferSize = 0;
};

#endif /* CLIENTSOCKET_H_ */