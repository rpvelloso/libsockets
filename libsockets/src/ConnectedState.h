/*
 * ConnectedState1.h
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SRC_CONNECTEDSTATE_H_
#define SRC_CONNECTEDSTATE_H_

#include "SocketState.h"

class ConnectedState : public SocketState {
public:
	ConnectedState(SocketImpl &impl);
	virtual ~ConnectedState();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<ClientSocket> acceptConnection() override;
	std::string getPort() override;
	size_t getSendBufferSize() override;
	size_t getReceiveBufferSize() override;
};

#endif /* SRC_CONNECTEDSTATE_H_ */
