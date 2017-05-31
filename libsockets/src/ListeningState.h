/*
 * ListeningState.h
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#ifndef LISTENINGSTATE_H_
#define LISTENINGSTATE_H_

#include "SocketState.h"

class ListeningState : public SocketState {
public:
	ListeningState(std::shared_ptr<SocketImpl> impl);
	virtual ~ListeningState();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<ClientSocket> acceptConnection() override;
	std::string getPort() override;
	size_t getSendBufferSize() override;
};

#endif /* LISTENINGSTATE_H_ */
