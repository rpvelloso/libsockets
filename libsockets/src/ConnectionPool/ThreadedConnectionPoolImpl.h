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

#ifndef SRC_CONNECTIONPOOL_THREADEDCONNECTIONPOOLIMPL_H_
#define SRC_CONNECTIONPOOL_THREADEDCONNECTIONPOOLIMPL_H_

#include "ConnectionPool/ConnectionPoolImpl.h"

namespace socks {

class ThreadedConnectionPoolImpl: public ConnectionPoolImpl {
public:
	ThreadedConnectionPoolImpl() = delete;
	ThreadedConnectionPoolImpl(
			ClientCallback readCallback,
			ClientCallback connectCallback = defaultCallback,
			ClientCallback disconnectCallback = defaultCallback,
			ClientCallback writeCallback = defaultCallback);
	virtual ~ThreadedConnectionPoolImpl();
	void addClientSocket(
			std::unique_ptr<ClientSocket> clientSocket,
			ClientCallback readCallback,
			ClientCallback connectCallback = defaultCallback,
			ClientCallback disconnectCallback = defaultCallback,
			ClientCallback writeCallback = defaultCallback) override;
	void addClientSocket(std::unique_ptr<ClientSocket> clientSocket) override;
private:
	ClientCallback readCB, connectCB, disconnectCB, writeCB;
};

} /* namespace socks */

#endif /* SRC_CONNECTIONPOOL_THREADEDCONNECTIONPOOLIMPL_H_ */
