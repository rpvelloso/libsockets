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

#ifndef LINUX_FACTORIES_LINUXSOCKETFACTORY_H_
#define LINUX_FACTORIES_LINUXSOCKETFACTORY_H_

#include "Factory/SocketFactoryImpl.h"

namespace socks {

class LinuxSocketFactory: public SocketFactoryImpl {
public:
	LinuxSocketFactory();
	virtual ~LinuxSocketFactory();
	SocketImpl *createSocketImpl() override;
	SocketImpl *createUDPSocketImpl() override;
	Poll *createPoll() override;
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> >
		createSocketPair() override;
	SocketAddressImpl *createSocketAddressImpl(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol) override;
};

}
#endif /* LINUX_FACTORIES_LINUXSOCKETFACTORY_H_ */
