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
	std::unique_ptr<ClientSocket> CreateClientSocket();
	std::unique_ptr<ServerSocket> CreateServerSocket();
	std::unique_ptr<Multiplexer> CreateMultiplexer(std::function<bool(std::shared_ptr<ClientSocket>)> callback);
};

#endif /* WINDOWSSOCKETFACTORY_H_ */
