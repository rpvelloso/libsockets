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

#include "ListeningState.h"

namespace socks {

ListeningState::ListeningState(SocketImpl &impl) : SocketState(impl) {
	setSocketState(SocketStateType::Listening);
	impl.setSocketState(socketState);
}

ListeningState::~ListeningState() {

}

void ListeningState::disconnect() {
	impl.disconnect();
}

std::unique_ptr<SocketImpl> ListeningState::acceptConnection() {
	return impl.acceptConnection();
}

std::string ListeningState::getPort() {
	return impl.getPort();
}

}
