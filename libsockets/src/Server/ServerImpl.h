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

#include <atomic>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include "ConnectionPool/ConnectionPool.h"
#include "Socket/BufferedClientSocket.h"
#include "Socket/ServerSocket.h"

namespace socks {

class ServerImplInterface {
public:
	ServerImplInterface() {};
	virtual ~ServerImplInterface() {};
	virtual void listen(const std::string &bindAddr, const std::string &port) = 0;
	virtual void stop() = 0;
	virtual std::string getPort() = 0;
};

class ServerImplBase : public ServerImplInterface {
public:
	ServerImplBase(ServerSocket *serverSocket):
		serverSocket(serverSocket)
	{}

	virtual void accept(ClientSocket socket) = 0;

	virtual bool errorCallback(std::exception & failure) {
		std::cerr << failure.what() << std::endl;
		return true;
	}

	void listen(const std::string &bindAddr, const std::string &port) override {
		wakeHost = bindAddr;
		listening = true;
		serverSocket->listenForConnections(bindAddr, port);
		while (listening) {
			try {
				ClientSocket clientSocket = serverSocket->acceptConnection();
				// never serve the connection stop() dials to wake us up
				if( ! listening ) break;
				accept(std::move(clientSocket));
			} catch (std::exception & error) {
				if( ! listening ) break;  // stop() during the last accept
				// one failure is a client that reset a pending connection
				// before we could accept it; only a listener that never
				// accepts anything again is really broken
				if( ! errorCallback(error) ) break;
			}
		}
	}

	// sets the flag and, since the blocked accept() never looks at it,
	// wakes the loop up by dialing the server from ourselves
	void stop() override {
		if( ! listening.exchange(false) ) return;
		std::string host = wakeHost;
		if( host.empty() || host == "0.0.0.0" ) host = "127.0.0.1";
		else if( host == "::" ) host = "::1";
		factory::makeClientSocket().connectTo(host, serverSocket->getPort());
	}

	std::string getPort() override {
		return serverSocket->getPort();
	}

private:
	std::unique_ptr<ServerSocket> serverSocket;
	std::atomic<bool> listening{false};
	std::string wakeHost;
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

	// an exception escaping the callback would reach std::terminate and
	// take the whole server down; the connection it was serving simply
	// ends, and the error is reported
	void accept(socks::ClientSocket socket) override {
		std::thread th(
			[this, socket = std::move(socket)]() mutable {
				try {
					socketCallback(std::move(socket));
				} catch (std::exception & error) {
					std::cerr << error.what() << std::endl;
				} catch (...) {
					std::cerr << "unknown exception thrown while serving a connection" << std::endl;
				}
			});
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
