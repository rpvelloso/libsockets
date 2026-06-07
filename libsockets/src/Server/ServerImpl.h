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

#ifndef SRC_SERVER_SERVERIMPL_H_
#define SRC_SERVER_SERVERIMPL_H_

#include <memory>
#include <thread>
#include "ConnectionPool/ConnectionPool.h"
#include "Socket/BufferedClientSocket.h"
#include "Socket/ServerSocket.h"

namespace socks {

class ServerImplInterface {
public:
	ServerImplInterface() {};
	virtual ~ServerImplInterface() {};
	virtual void listen(const std::string &bindAddr, const std::string &port) = 0;
	virtual std::string getPort() = 0;
};

class ServerImplBase : public ServerImplInterface {
public:
	ServerImplBase(ServerSocket *serverSocket):
		serverSocket(serverSocket)
	{}

	virtual void accept(ClientSocket socket) = 0;

	void listen(const std::string &bindAddr, const std::string &port) override {
		serverSocket->listenForConnections(bindAddr, port);
		while (true) {
			accept(serverSocket->acceptConnection());
		}
	};

	std::string getPort() override {
		return serverSocket->getPort();
	};

private:
	std::unique_ptr<ServerSocket> serverSocket;
};

class SimpleServerImpl : public ServerImplBase {
	std::function<void(ClientSocket)> socketCallback;
public:
	SimpleServerImpl(
		std::function<void(ClientSocket)> socketCallback
	):
		socks::ServerImplBase(new ServerSocket()),
		socketCallback(socketCallback)
	{}

	void accept(socks::ClientSocket socket) override {
		std::thread th(socketCallback,std::move(socket));
		th.detach();
	}
};

template<class ClientContext>
class ServerImpl : public ServerImplBase {
public:
	ServerImpl(
		ServerSocket *serverSocket,
		ConnectionPool *connectionPool,
		ClientCallback<ClientContext> readCallback,
		ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) :
			ServerImplBase(serverSocket),
			connectionPool(connectionPool),
			readCB(readCallback),
			connectCB(connectCallback),
			disconnectCB(disconnectCallback),
			writeCB(writeCallback)
		{};

	virtual ~ServerImpl() = default;

	void accept(ClientSocket socket) override {
		std::unique_ptr<BufferedClientSocketInterface> clientSocket(
			new BufferedClientSocket<ClientContext>(
					std::make_unique<ClientSocket>(std::move(socket)),
					readCB,
					connectCB,
					disconnectCB,
					writeCB));
		connectionPool->addClientSocket(std::move(clientSocket));
	}

private:
	std::unique_ptr<ConnectionPool> connectionPool;
	ClientCallback<ClientContext> readCB, connectCB, disconnectCB, writeCB;
};

}
#endif /* SRC_SERVER_SERVERIMPL_H_ */
