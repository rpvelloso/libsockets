/*
 * WindowsSocketFactory.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "WindowsSocketFactory.h"
#include "WindowsSocket.h"

WindowsSocketFactory::WindowsSocketFactory() {
}

WindowsSocketFactory::~WindowsSocketFactory() {
}

std::unique_ptr<ClientSocket> WindowsSocketFactory::CreateClientSocket() {
	return std::make_unique<ClientSocket>(new WindowsSocket);
}

std::unique_ptr<ServerSocket> WindowsSocketFactory::CreateServerSocket() {
	return std::make_unique<ServerSocket>(new WindowsSocket);
}
