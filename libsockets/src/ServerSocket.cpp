/*
 * ServerSocket.cpp
 *
 *  Created on: 25 de mai de 2017
 *      Author: rvelloso
 */

#include "ServerSocket.h"
#include "ClientSocket.h"

ServerSocket::ServerSocket(std::shared_ptr<SocketImpl> impl) : Socket(impl) {
}

ServerSocket::~ServerSocket() {
}

int ServerSocket::listenForConnections(const std::string &bindAddr, const std::string &port) {
	int ret;

	if ((ret = state->reuseAddress()) != 0)
		return ret;

	if ((ret = state->listenForConnections(bindAddr, port)) == 0)
		state.reset(new ListeningState(impl));

	return ret;
}

std::unique_ptr<ClientSocket> ServerSocket::acceptConnection() {
	return state->acceptConnection();
}

void ServerSocket::disconnect() {
	state->disconnect();
	state.reset(new ClosedState(impl));
}
