/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Factory/WindowsFDFactory.h"
#include "Socket/WindowsSocket.h"

namespace socks {

FDFactory &TCPFDFactory() {
	static WindowsTCPFDFactory factory;
	return factory;
}

FDFactory &UDPFDFactory() {
	static WindowsUDPFDFactory factory;
	return factory;
}

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
