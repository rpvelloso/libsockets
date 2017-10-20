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

#ifndef SRC_FACTORY_SOCKETFACTORY_H_
#define SRC_FACTORY_SOCKETFACTORY_H_

#include <fstream>
#include <iostream>

#include <memory>
#include <atomic>

#include "Factory/SocketFactoryImpl.h"

namespace socks {

class SocketFactory {
public:
	SocketFactory() = delete;
	SocketFactory(SocketFactoryImpl *impl);

	SocketImpl *createSocketImpl();
	SocketImpl *createUDPSocketImpl();
	SocketAddressImpl *createSocketAddressImpl(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP);
	Poll *createPoll();
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair();
private:
	std::unique_ptr<SocketFactoryImpl> impl;
};

extern SocketFactory socketFactory;

}

#endif /* SRC_FACTORY_SOCKETFACTORY_H_ */
