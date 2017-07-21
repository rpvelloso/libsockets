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

namespace socks {

class ClientSocket : public Socket {
public:
	ClientSocket(ClientSocket &&) = default;
	ClientSocket &operator=(ClientSocket &&);

	ClientSocket(SocketImpl *impl);
	virtual ~ClientSocket();
	int receiveData(void *buf, size_t len);
	int sendData(const void *buf, size_t len);
	int connectTo(const std::string &host, const std::string &port);
	void disconnect();
	size_t getSendBufferSize() const;
	size_t getReceiveBufferSize() const;

private:
	size_t sendBufferSize = 0;
	size_t receiveBufferSize = 0;
};

}
#endif /* CLIENTSOCKET_H_ */
