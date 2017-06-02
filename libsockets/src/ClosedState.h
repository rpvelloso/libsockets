/*
 * ClosedState.h
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#ifndef CLOSEDSTATE_H_
#define CLOSEDSTATE_H_

#include "SocketState.h"

class ClosedState : public SocketState {
public:
	ClosedState(SocketImpl &impl);
	virtual ~ClosedState();
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

#endif /* CLOSEDSTATE_H_ */
