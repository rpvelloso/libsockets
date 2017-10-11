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

#ifndef SRC_SOCKET_SOCKETSTATE_SOCKETSTATE_H_
#define SRC_SOCKET_SOCKETSTATE_SOCKETSTATE_H_

#include <exception>

#include "Socket/SocketImpl.h"

namespace socks {

class SocketState : public SocketImpl {
public:
	SocketState(SocketImpl &impl) : SocketImpl(), impl(impl) {};
	virtual ~SocketState() {};
	int setNonBlockingIO(bool status) override {
		return impl.setNonBlockingIO(status);
	};
	int reuseAddress() override {
		return impl.reuseAddress();
	};
protected:
	SocketImpl &impl;
};

}
#endif /* SRC_SOCKET_SOCKETSTATE_SOCKETSTATE_H_ */
