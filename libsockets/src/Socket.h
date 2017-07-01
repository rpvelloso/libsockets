/*
 * Socket.h
 *
 *  Created on: 24 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <memory>

#include "SocketImpl.h"

#include "ConnectedState.h"
#include "DisconnectedState.h"
#include "ListeningState.h"
#include "ClosedState.h"

namespace socks {

class Socket {
public:
	Socket(Socket &&) = default;
	Socket(SocketImpl *impl) : impl(impl) {
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
	};
	virtual ~Socket() {
		if (state)
			state->disconnect();
	};
	virtual int setNonBlockingIO(bool status) {
		return impl->setNonBlockingIO(status);
	};
	virtual std::string getPort() {
		return impl->getPort();
	}
	virtual SocketImpl &getImpl() {
		return *impl;
	};
protected:
	std::unique_ptr<SocketImpl> impl;
	std::unique_ptr<SocketState> state;
};

}

#endif /* SOCKET_H_ */
