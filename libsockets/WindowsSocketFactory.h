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
};

#endif /* WINDOWSSOCKETFACTORY_H_ */
