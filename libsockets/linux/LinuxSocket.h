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
#include "defs.h"

namespace socks {

class LinuxMultiplexer;

class LinuxSocket: public SocketImpl {
	friend class LinuxSocketFactory;
public:
	LinuxSocket(FDFactory &fdFactory = TCPFDFactory);
	virtual ~LinuxSocket();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int bindSocket(const std::string &bindAddr, const std::string &port) override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<SocketImpl> acceptConnection() override;
	int setNonBlockingIO(bool status) override;
	int reuseAddress() override;
	std::string getPort() override;
	size_t getSendBufferSize() override;
	size_t getReceiveBufferSize() override;
private:
	std::string port = "";

	LinuxSocket(SocketFDType); // ctor used by acceptConnections() & LinuxMultiplexer
};

}
#endif /* LINUXSOCKET_H_ */
