/*
 * DisconnectedState.h
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#ifndef DISCONNECTEDSTATE_H_
#define DISCONNECTEDSTATE_H_

#include "SocketState.h"

namespace socks {

class DisconnectedState : public SocketState {
public:
	DisconnectedState(SocketImpl &impl);
	virtual ~DisconnectedState();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<SocketImpl> acceptConnection() override;
	std::string getPort() override;
	size_t getSendBufferSize() override;
	size_t getReceiveBufferSize() override;
};

}

#endif /* DISCONNECTEDSTATE_H_ */
