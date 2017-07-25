/*
 * WindowsSocketFactory.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSSOCKETFACTORY_H_
#define WINDOWSSOCKETFACTORY_H_

#include "SocketFactory.h"

namespace socks {

class WindowsSocketFactory: public SocketFactory {
public:
	WindowsSocketFactory();
	virtual ~WindowsSocketFactory();
	SocketImpl *createSocketImpl() override;
	SocketImpl *createUDPSocketImpl() override;
	SocketImpl *createSSLSocketImpl() override;
	ClientSocket createClientSocket() override;
	ClientSocket createUDPClientSocket() override;
	ClientSocket createSSLClientSocket() override;
	ServerSocket createServerSocket() override;
	ServerSocket createSSLServerSocket() override;
	Multiplexer createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback) override;
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() override;
	SocketAddress createAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol) override;
	DatagramSocket createDatagramSocket() override;
};

}
#endif /* WINDOWSSOCKETFACTORY_H_ */
