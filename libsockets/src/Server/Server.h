/*
 * Server.h
 *
 *  Created on: 11 de out de 2017
 *      Author: rvelloso
 */

#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <memory>

#include "Server/ServerImpl.h"

namespace socks {

class Server {
public:
	Server(ServerImpl *impl) : impl(impl) {};
	void listen() {impl->listen();};
private:
	std::unique_ptr<ServerImpl> impl;
};

}
#endif /* SRC_SERVER_SERVER_H_ */
