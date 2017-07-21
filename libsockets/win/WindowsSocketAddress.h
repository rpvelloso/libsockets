/*
 * WindowsSocketAddress.h
 *
 *  Created on: 21 de jul de 2017
 *      Author: rvelloso
 */

#ifndef WIN_WINDOWSSOCKETADDRESS_H_
#define WIN_WINDOWSSOCKETADDRESS_H_

#include "SocketAddress.h"
#include "WindowsSocket.h"

namespace socks {

class WindowsSocketAddress : public SocketAddress {
public:
	WindowsSocketAddress(WindowsSocketAddress &&) = default;
	virtual WindowsSocketAddress& operator=(WindowsSocketAddress &&) = default;
	WindowsSocketAddress(struct sockaddr *addr, size_t addrSize);
	WindowsSocketAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP);
	virtual ~WindowsSocketAddress();

	void *getSocketAddress() const override;
	void setSocketAddressSize(int saSize) override;
	int getSocketAddressSize() const override;
private:
	WindowsSocketAddress() = delete;

	void setSocketAddress(struct sockaddr *sa, size_t saSize);

	std::unique_ptr<struct sockaddr_storage> sockAddrPtr;
	int sockAddrSize = sizeof(struct sockaddr_storage);
};

} /* namespace socks */

#endif /* WIN_WINDOWSSOCKETADDRESS_H_ */
