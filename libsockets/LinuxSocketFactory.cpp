/*
 * LinuxSocketFactory.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "LinuxSocketFactory.h"
#include "LinuxSocket.h"

SocketFactory *socketFactory = new LinuxSocketFactory();

LinuxSocketFactory::LinuxSocketFactory() {
}

LinuxSocketFactory::~LinuxSocketFactory() {
}

std::unique_ptr<ClientSocket> LinuxSocketFactory::CreateClientSocket() {
	return std::make_unique<ClientSocket>(new LinuxSocket);
}

std::unique_ptr<ServerSocket> LinuxSocketFactory::CreateServerSocket() {
	return std::make_unique<ServerSocket>(new LinuxSocket);
}