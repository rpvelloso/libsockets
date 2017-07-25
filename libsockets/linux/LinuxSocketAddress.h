/*
 * WindowsSocketAddress.h
 *
 *  Created on: 21 de jul de 2017
 *      Author: rvelloso
 */

#ifndef WIN_LINUXSOCKETADDRESS_H_
#define WIN_LINUXSOCKETADDRESS_H_

#include "SocketAddress.h"
#include "LinuxSocket.h"

namespace socks {

class LinuxSocketAddress : public SocketAddress {
public:
	LinuxSocketAddress(LinuxSocketAddress &&) = default;
	virtual LinuxSocketAddress& operator=(LinuxSocketAddress &&) = default;
	LinuxSocketAddress(struct sockaddr *addr, size_t addrSize);
	LinuxSocketAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP);
	virtual ~LinuxSocketAddress();

	void *getSocketAddress() const override;
	void setSocketAddressSize(int saSize) override;
	int getSocketAddressSize() const override;
	bool operator==(const SocketAddress &rhs) override;
	std::string getHostname() const override;
	std::string getPort() const override;
private:
	LinuxSocketAddress() = delete;

	void setSocketAddress(struct sockaddr *sa, size_t saSize);

	std::unique_ptr<struct sockaddr_storage> sockAddrPtr;
	int sockAddrSize = sizeof(struct sockaddr_storage);
	std::string hostname, port;
};

} /* namespace socks */

#endif /* WIN_LINUXSOCKETADDRESS_H_ */
