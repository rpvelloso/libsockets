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
	ClientSocket createClientSocket();
	ClientSocket createSSLClientSocket() override;
	ServerSocket createServerSocket() override;
	ServerSocket createSSLServerSocket() override;
	std::unique_ptr<ClientSocket> createClientSocketPtr() override;
	std::unique_ptr<ClientSocket> createSSLClientSocketPtr() override;
	std::unique_ptr<ServerSocket> createServerSocketPtr() override;
	std::unique_ptr<ServerSocket> createSSLServerSocketPtr() override;
	std::unique_ptr<Multiplexer> createMultiplexerPtr(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback) override;
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() override;
};

}
#endif /* WINDOWSSOCKETFACTORY_H_ */
