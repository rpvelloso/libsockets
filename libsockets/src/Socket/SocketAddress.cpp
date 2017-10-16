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
#include "Socket/SocketAddress.h"

namespace socks {

namespace factory {
	SocketAddress makeSocketAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol) {

		return SocketAddress(socketFactory.createSocketAddressImpl(host, port, protocol));
	};
}

SocketAddress::SocketAddress(SocketAddressImpl *impl) : impl(impl) {

};

void *SocketAddress::getSocketAddress() const {
	return impl->getSocketAddress();
};

void SocketAddress::setSocketAddressSize(int saSize) {
	impl->setSocketAddressSize(saSize);
};

int SocketAddress::getSocketAddressSize() const {
	return impl->getSocketAddressSize();
};

bool SocketAddress::operator==(const SocketAddress &rhs) {
	return impl->operator==(*rhs.impl.get());
};

std::string SocketAddress::getHostname() const {
	return impl->getHostname();
};

std::string SocketAddress::getPort() const {
	return impl->getPort();
};

}





