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
	std::unique_ptr<ClientSocket> CreateClientSocket();
	std::unique_ptr<ServerSocket> CreateServerSocket();
	std::unique_ptr<Multiplexer> CreateMultiplexer(MultiplexerCallback callback);
};

#endif /* LINUXSOCKETFACTORY_H_ */
