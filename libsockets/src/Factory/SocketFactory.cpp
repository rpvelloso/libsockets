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

namespace socks {

SocketFactory::SocketFactory(SocketFactoryImpl *impl) : impl(impl) {};

std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> >
SocketFactory::createSocketPair() {
	return impl->createSocketPair();
};

SocketAddressImpl *SocketFactory::createSocketAddressImpl(
		const std::string &host,
		const std::string &port,
		SocketProtocol protocol) {

	return impl->createSocketAddressImpl(host, port, protocol);
};

size_t SocketFactory::createID() {
	static std::atomic<std::size_t> id(0);

	return ++id;
}

SocketImpl* SocketFactory::createSocketImpl() {
	return impl->createSocketImpl();
}

SocketImpl* SocketFactory::createUDPSocketImpl() {
	return impl->createUDPSocketImpl();
}

Poll *SocketFactory::createPoll() {
	return impl->createPoll();
}

}
