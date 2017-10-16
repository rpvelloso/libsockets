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
#include "Socket/ClientSocket.h"
#include "Socket/ServerSocket.h"

namespace socks {

namespace factory {
	ServerSocket makeServerSocket() {
		return ServerSocket(socketFactory.createSocketImpl());
	}
}

ServerSocket::ServerSocket(SocketImpl *impl) : Socket(impl) {
}

ServerSocket::ServerSocket() : Socket(socketFactory.createSocketImpl()) {
}

ServerSocket::~ServerSocket() {
}

int ServerSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
	int ret;

	if ((ret = state->reuseAddress()) != 0)
		return ret;

	if ((ret = state->listenForConnections(bindAddr, port)) == 0)
		state.reset(new ListeningState(getImpl()));

	return ret;
}

ClientSocket ServerSocket::acceptConnection() {
	return ClientSocket(state->acceptConnection().release());
}

void ServerSocket::disconnect() {
	state->disconnect();
	state.reset(new ClosedState(getImpl()));
}

}
