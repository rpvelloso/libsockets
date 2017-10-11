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

#ifndef WIN_FACTORIES_WINDOWSSOCKETFACTORY_H_
#define WIN_FACTORIES_WINDOWSSOCKETFACTORY_H_

#include "Factory/SocketFactory.h"

namespace socks {

class WindowsSocketFactory: public SocketFactoryImpl {
public:
	WindowsSocketFactory();
	virtual ~WindowsSocketFactory();
	SocketImpl *createSocketImpl() override;
	SocketImpl *createUDPSocketImpl() override;
	SocketImpl *createSSLSocketImpl() override;
	Multiplexer createMultiplexer(
			MultiplexerCallback readCallback,
			MultiplexerCallback connectCallback,
			MultiplexerCallback disconnectCallback,
			MultiplexerCallback writeCallback) override;
	std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() override;
	SocketAddress createAddress(
			const std::string &host,
			const std::string &port,
			SocketProtocol protocol) override;
};

}
#endif /* WIN_FACTORIES_WINDOWSSOCKETFACTORY_H_ */
