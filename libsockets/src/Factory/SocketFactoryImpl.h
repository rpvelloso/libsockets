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

#ifndef SRC_FACTORY_SOCKETFACTORYIMPL_H_
#define SRC_FACTORY_SOCKETFACTORYIMPL_H_

#include "Factory/Poll.h"
#include "Socket/SocketImpl.h"
#include "Socket/ClientSocket.h"
#include "Socket/SocketAddressImpl.h"

namespace socks {

class SocketFactoryImpl {
public:
	SocketFactoryImpl() {};
	virtual ~SocketFactoryImpl() {};
	virtual SocketImpl *createSocketImpl() = 0;
	virtual SocketImpl *createUDPSocketImpl() = 0;
	virtual Poll *createPoll() = 0;
	virtual SocketAddressImpl *createSocketAddressImpl(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol = SocketProtocol::UDP) = 0;
	virtual std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() = 0;
};

}

#endif /* SRC_FACTORY_SOCKETFACTORYIMPL_H_ */
