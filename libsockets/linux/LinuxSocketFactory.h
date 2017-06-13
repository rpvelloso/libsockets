/*
 * LinuxSocketFactory.h
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#ifndef LINUXSOCKETFACTORY_H_
#define LINUXSOCKETFACTORY_H_

#include "SocketFactory.h"
#include "Multiplexer.h"

class LinuxSocketFactory: public SocketFactory {
public:
	LinuxSocketFactory();
	virtual ~LinuxSocketFactory();
	std::unique_ptr<ClientSocket> createClientSocket() override;
	std::unique_ptr<ClientSocket> createSSLClientSocket() override;
	std::unique_ptr<ServerSocket> createServerSocket() override;
	std::unique_ptr<ServerSocket> createSSLServerSocket() override;
	std::unique_ptr<Multiplexer> createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback) override;
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() override;
};

#endif /* LINUXSOCKETFACTORY_H_ */
