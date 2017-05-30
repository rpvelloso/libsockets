/*
 * SocketState.h
 *
 *  Created on: 30 de mai de 2017
 *      Author: rvelloso
 */

#ifndef SRC_SOCKETSTATE_H_
#define SRC_SOCKETSTATE_H_

#include <exception>
#include "SocketImpl.h"

class SocketState : public SocketImpl {
public:
	SocketState(std::shared_ptr<SocketImpl> impl) : SocketImpl(), impl(impl) {};
	virtual ~SocketState() {};
	int setNonBlockingIO(bool status) override {
		return impl->setNonBlockingIO(status);
	};
	int reuseAddress() override {
		return impl->reuseAddress();
	};
protected:
	std::shared_ptr<SocketImpl> impl;
};

#endif /* SRC_SOCKETSTATE_H_ */
