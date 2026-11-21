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

#include "SocketAddress.h"
#include "SocketAddressImpl.h"
#include "Factory/SocketFactory.h"

namespace socks {

namespace factory {
	SocketAddress makeSocketAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol) {

		return SocketAddress(socketFactory().createSocketAddressImpl(host, port, protocol));
	};
}

SocketAddress::SocketAddress(SocketAddressImpl *impl) : impl(impl) {};

SocketAddress::SocketAddress(SocketAddress &&) = default;
SocketAddress::~SocketAddress() = default;

// an address without an implementation is the "no address" value that failed
// operations return; every accessor tolerates it
void *SocketAddress::getSocketAddress() const {
	return impl ? impl->getSocketAddress() : nullptr;
};

void SocketAddress::setSocketAddressSize(int saSize) {
	if (impl) impl->setSocketAddressSize(saSize);
};

int SocketAddress::getSocketAddressSize() const {
	return impl ? impl->getSocketAddressSize() : 0;
};

bool SocketAddress::operator==(const SocketAddress &rhs) {
	if (!impl || !rhs.impl) return impl.get() == rhs.impl.get();
	return impl->operator==(*rhs.impl.get());
};

bool SocketAddress::operator!=(const SocketAddress &rhs) {
	return !(*this == rhs);
};

std::string SocketAddress::getHostname() const {
	return impl ? impl->getHostname() : "";
};

std::string SocketAddress::getPort() const {
	return impl ? impl->getPort() : "";
};

}





