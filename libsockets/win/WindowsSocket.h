/*
 * WindowsSocket.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSSOCKET_H_
#define WINDOWSSOCKET_H_

#include "defs.h"
#include "Socket.h"
#include "SocketImpl.h"

#include <winsock2.h>
#include <windows.h>
#include <winsock.h>
#include <ws2tcpip.h>

class WindowsSocket: public SocketImpl {
public:
	WindowsSocket();
	virtual ~WindowsSocket();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<SocketImpl> acceptConnection() override;
	int setNonBlockingIO(bool status) override;
	int reuseAddress() override;
	std::string getPort() override;
	size_t getSendBufferSize() override;
	size_t getReceiveBufferSize() override;
private:
	WindowsSocket(SocketFDType); // ctor used by acceptConnections()
	std::string port = "";
};

extern int winSockInit();
extern void winSockCleanup();

#endif /* WINDOWSSOCKET_H_ */
