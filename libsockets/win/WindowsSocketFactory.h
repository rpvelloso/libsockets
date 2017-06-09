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
	std::unique_ptr<ClientSocket> CreateClientSocket() override;
	std::unique_ptr<ClientSocket> CreateSSLClientSocket() override;
	std::unique_ptr<ServerSocket> CreateServerSocket() override;
	std::unique_ptr<ServerSocket> CreateSSLServerSocket() override;
	std::unique_ptr<Multiplexer> CreateMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback) override;
};

#endif /* WINDOWSSOCKETFACTORY_H_ */
