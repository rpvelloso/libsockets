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

class LinuxSocketFactory: public SocketFactoryImpl {
public:
	LinuxSocketFactory();
	virtual ~LinuxSocketFactory();
	SocketImpl *createSocketImpl() override;
	SocketImpl *createUDPSocketImpl() override;
	SocketImpl *createSSLSocketImpl() override;
	Multiplexer createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback) override;
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> >
		createSocketPair() override;
	SocketAddress createAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol) override;
};

}
#endif /* LINUXSOCKETFACTORY_H_ */
