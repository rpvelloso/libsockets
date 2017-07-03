/*
 * WindowsFDFactory.cpp
 *
 *  Created on: 3 de jul de 2017
 *      Author: rvelloso
 */

#include "WindowsFDFactory.h"
#include "WindowsSocket.h"

namespace socks {

FDFactory TCPFDFactory(new WindowsTCPFDFactory());
FDFactory UDPFDFactory(new WindowsUDPFDFactory());

WindowsTCPFDFactory::WindowsTCPFDFactory() : FDFactory() {
}

WindowsTCPFDFactory::~WindowsTCPFDFactory() {
}

SocketFDType WindowsTCPFDFactory::operator ()() {
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

WindowsUDPFDFactory::WindowsUDPFDFactory() : FDFactory() {
}

WindowsUDPFDFactory::~WindowsUDPFDFactory() {
}

SocketFDType WindowsUDPFDFactory::operator ()() {
	return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

}
