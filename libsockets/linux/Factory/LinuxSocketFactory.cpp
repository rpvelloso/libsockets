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

#include "Factory/SocketFactory.h"
#include "Factory/LinuxSocketFactory.h"
#include "Socket/LinuxSocket.h"
#include "Socket/SSL/OpenSSLSocket.h"
#include "Multiplexer/LinuxPoll.h"
#include "Multiplexer/LinuxSelect.h"
#include "Socket/LinuxSocketAddress.h"

namespace socks {

SocketFactory socketFactory(new LinuxSocketFactory());

LinuxSocketFactory::LinuxSocketFactory() {
}

LinuxSocketFactory::~LinuxSocketFactory() {
}

SocketImpl *LinuxSocketFactory::createSocketImpl() {
	return new LinuxSocket();
}

SocketImpl *LinuxSocketFactory::createUDPSocketImpl() {
	return new LinuxSocket(UDPFDFactory);
}

Poll *LinuxSocketFactory::createPoll() {
	return new LinuxPoll();
}

std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > LinuxSocketFactory::createSocketPair() {
	int selfPipe[2];

	socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, selfPipe);
	auto sockIn = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[0]));
	auto sockOut = std::make_unique<ClientSocket>(new LinuxSocket(selfPipe[1]));
	return std::make_pair(std::move(sockIn), std::move(sockOut));
}

SocketAddressImpl *LinuxSocketFactory::createSocketAddressImpl(
		const std::string& host,
		const std::string& port,
		SocketProtocol protocol) {
	return new LinuxSocketAddress(host, port, protocol);
}

}
