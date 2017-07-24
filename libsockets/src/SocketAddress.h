/*
 * SocketAddress.h
 *
 *  Created on: 21 de jul de 2017
 *      Author: rvelloso
 */

#ifndef SRC_SOCKETADDRESS_H_
#define SRC_SOCKETADDRESS_H_

#include <memory>

namespace socks {

enum class SocketProtocol {
	TCP,
	UDP
};

class SocketAddress {
public:
	SocketAddress(SocketAddress &&) = default;
	virtual SocketAddress &operator=(SocketAddress &&) = default;
	SocketAddress(SocketAddress *impl) : impl(impl) {};
	virtual ~SocketAddress() {};
	virtual void *getSocketAddress() const { return impl->getSocketAddress(); };
	virtual void setSocketAddressSize(int saSize) { impl->setSocketAddressSize(saSize); };
	virtual int getSocketAddressSize() const { return impl->getSocketAddressSize(); };
	virtual bool operator==(const SocketAddress &rhs) { return impl->operator==(rhs); };
	virtual std::string getHostname() { return impl->getHostname(); };
	virtual std::string getPort() { return impl->getPort(); };
protected:
	SocketAddress() {};
	std::unique_ptr<SocketAddress> impl;
};

}

#endif /* SRC_SOCKETADDRESS_H_ */
