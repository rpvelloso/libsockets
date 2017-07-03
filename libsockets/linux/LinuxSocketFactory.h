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

namespace socks {

class LinuxSocketFactory: public SocketFactory {
public:
	LinuxSocketFactory();
	virtual ~LinuxSocketFactory();
	ClientSocket createClientSocket() override;
	ClientSocket createUDPClientSocket() override;
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
#endif /* LINUXSOCKETFACTORY_H_ */
