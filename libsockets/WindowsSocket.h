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

//#define WINVER _WIN32_WINNT_WIN7
//#define _WIN32_WINNT 0x0600
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
	ClientSocket *acceptConnection() override;
	int setNonBlockingIO(bool status) override;
	int reuseAddress() override;
private:
	WindowsSocket(SOCKET); // ctor used by acceptConnections()
	SOCKET fd;
};

extern int winSockInit();
extern void winSockCleanup();

#endif /* WINDOWSSOCKET_H_ */