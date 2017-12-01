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

#include <memory>

#include "Socket/Socket.h"
#include "Socket/SocketState/ClosedState.h"
#include "Socket/SocketState/ConnectedState.h"
#include "Socket/SocketState/DisconnectedState.h"
#include "Socket/SocketState/ListeningState.h"

namespace socks {

Socket::Socket(SocketImpl *impl) : impl(impl) {
	switch (impl->getSocketState()) {
	case SocketStateType::Disconnected:
		state.reset(new DisconnectedState(getImpl()));
		break;
	case SocketStateType::Connected:
		state.reset(new ConnectedState(getImpl()));
		break;
	case SocketStateType::Listening:
		state.reset(new ListeningState(getImpl()));
		break;
	case SocketStateType::Closed:
		state.reset(new ClosedState(getImpl()));
		break;
	}
}

Socket::~Socket() {
	if (state)
		state->disconnect();
}

int Socket::setNonBlockingIO(bool status) {
	return impl->setNonBlockingIO(status);
}

std::string Socket::getPort() {
	return impl->getPort();
}

SocketImpl &Socket::getImpl() {
	return *impl;
}

SocketAddress Socket::getLocalAddress() {
	return impl->getLocalAddress();
}

SocketAddress Socket::getRemoteAddress() {
	return impl->getRemoteAddress();
}

}
