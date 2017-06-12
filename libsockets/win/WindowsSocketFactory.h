/*
 * WindowsSocketFactory.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWSSOCKETFACTORY_H_
#define WINDOWSSOCKETFACTORY_H_

#include "SocketFactory.h"

class WindowsSocketFactory: public SocketFactory {
public:
	WindowsSocketFactory();
	virtual ~WindowsSocketFactory();
	std::unique_ptr<ClientSocket> createClientSocket() override;
	std::unique_ptr<ClientSocket> createSSLClientSocket() override;
	std::unique_ptr<ServerSocket> createServerSocket() override;
	std::unique_ptr<ServerSocket> createSSLServerSocket() override;
	std::unique_ptr<Multiplexer> createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback) override;
};

#endif /* WINDOWSSOCKETFACTORY_H_ */
