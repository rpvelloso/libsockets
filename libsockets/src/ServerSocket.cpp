/*
 * ServerSocket.cpp
 *
 *  Created on: 25 de mai de 2017
 *      Author: rvelloso
 */

#include "ServerSocket.h"
#include "ClientSocket.h"
#include "SocketFactory.h"

namespace socks {

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
