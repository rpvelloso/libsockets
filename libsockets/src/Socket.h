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
#include "SocketState.h"

class Socket {
public:
	Socket(std::shared_ptr<SocketImpl> impl) : impl(impl) {
		switch (impl->getSocketState()) {
		case SocketStateType::Disconnected:
			state.reset(new DisconnectedState(impl));
			break;
		case SocketStateType::Connected:
			state.reset(new ConnectedState(impl));
			break;
		case SocketStateType::Listening:
			state.reset(new ListeningState(impl));
			break;
		case SocketStateType::Closed:
			state.reset(new ClosedState(impl));
			break;
		}
	};
	virtual ~Socket() {};
	virtual int setNonBlockingIO(bool status) {
		return impl->setNonBlockingIO(status);
	};
	virtual std::string getPort() {
		return impl->getPort();
	}
	virtual std::shared_ptr<SocketImpl> getImpl() {
		return impl;
	};
protected:
	std::shared_ptr<SocketImpl> impl;
	std::shared_ptr<SocketState> state;
};

#endif /* SOCKET_H_ */
