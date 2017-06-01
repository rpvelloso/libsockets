/*
 * LinuxSocket.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef LINUXSOCKET_H_
#define LINUXSOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include "Socket.h"
#include "SocketImpl.h"

class LinuxMultiplexer;

class LinuxSocket: public SocketImpl {
	friend class LinuxMultiplexer;
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
	std::string getPort() override;
	size_t getSendBufferSize() override;
	size_t getReceiveBufferSize() override;

	/* Implementation specific!!! FD data type changes from one OS to another.
	 * the FD is needed by multiplexer class.
	 * */
	int getFD();
private:
	int fd;
	std::string port = "";

	LinuxSocket(int); // ctor used by acceptConnections() & LinuxMultiplexer
};

#endif /* LINUXSOCKET_H_ */
