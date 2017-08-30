/*
 * WindowsFDFactory.cpp
 *
 *  Created on: 3 de jul de 2017
 *      Author: rvelloso
 */

#include "LinuxFDFactory.h"
#include "LinuxSocket.h"

namespace socks {

FDFactory TCPFDFactory(new LinuxTCPFDFactory());
FDFactory UDPFDFactory(new LinuxUDPFDFactory());

LinuxTCPFDFactory::LinuxTCPFDFactory() : FDFactory() {
}

LinuxTCPFDFactory::~LinuxTCPFDFactory() {
}

SocketFDType LinuxTCPFDFactory::operator ()() {
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

LinuxUDPFDFactory::LinuxUDPFDFactory() : FDFactory() {
}

LinuxUDPFDFactory::~LinuxUDPFDFactory() {
}

SocketFDType LinuxUDPFDFactory::operator ()() {
	return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

}
