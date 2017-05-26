/*
 * WindowsSocket.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSSOCKET_H_
#define WINDOWSSOCKET_H_

#include "Socket.h"
#include "SocketImpl.h"
#include "SocketFactory.h"

class LinuxSocket: public SocketImpl {
public:
	LinuxSocket();
	virtual ~LinuxSocket();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<ClientSocket> acceptConnection() override;
	int setNonBlockingIO(bool status) override;
	int reuseAddress() override;
private:
	LinuxSocket(int); // ctor used by acceptConnections()
	int fd;
};

extern SocketFactory *socketFactory;

#endif /* WINDOWSSOCKET_H_ */
