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

#ifndef SRC_SOCKET_SSL_OPENSSLSOCKET_H_
#define SRC_SOCKET_SSL_OPENSSLSOCKET_H_

#include <functional>
#include <openssl/ssl.h>

#include "ConnectionPool/ThreadedConnectionPoolImpl.h"
#include "ConnectionPool/MultiplexedConnectionPoolImpl.h"
#include "Factory/SocketFactory.h"
#include "Server/Server.h"
#include "Socket/ClientSocket.h"
#include "Socket/ServerSocket.h"
#include "Socket/SocketStream.h"
#include "Socket/SocketImpl.h"

namespace socks {

struct FreeSSLContext {
	void operator()(SSL_CTX* sslContext) const {
		if (sslContext != nullptr) SSL_CTX_free(sslContext);
	}
};

struct FreeSSLHandler {
	void operator()(SSL* sslHandler) const {
		if (sslHandler != nullptr) {
			SSL_shutdown(sslHandler);
			SSL_free(sslHandler);
		}
	}
};

using SSLMethodType = std::function<decltype(DTLS_server_method)>;
using SSLCtxPtr = std::unique_ptr<SSL_CTX, FreeSSLContext>;
using SSLHandlerPtr = std::unique_ptr<SSL, FreeSSLHandler>;

class OpenSSLSocket: public SocketImpl {
public:
	OpenSSLSocket(SocketImpl *impl);
	virtual ~OpenSSLSocket();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
	std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len) override;
	int sendTo(const SocketAddress &addr, const void *buf, size_t len) override;
	int connectTo(const std::string &host, const std::string &port) override;
	void disconnect() override;
	int bindSocket(const std::string &bindAddr, const std::string &port) override;
	int listenForConnections(const std::string &bindAddr, const std::string &port) override;
	std::unique_ptr<SocketImpl> acceptConnection() override;
	int setNonBlockingIO(bool status) override;
	int reuseAddress() override;
	std::string getPort() override;
	size_t getSendBufferSize() override;
	size_t getReceiveBufferSize() override;
	SocketStateType getSocketState() override;
	void setSocketState(SocketStateType socketState) override;
	SocketFDType getFD() override;
	SocketAddress getLocalAddress() override;
	SocketAddress getRemoteAddress() override;
private:
	OpenSSLSocket(SocketFDType fd, SocketImpl *impl, SSL_CTX *sslContext);

	std::unique_ptr<SocketImpl> impl;
	SSLCtxPtr sslContext;
	SSLHandlerPtr sslHandler;
	SSLMethodType serverMethod = TLS_server_method;
	SSLMethodType clientMethod = TLS_client_method;
};

namespace factory {
	ClientSocket makeSSLClientSocket();
	ServerSocket makeSSLServerSocket();
	SocketStream makeSSLSocketStream();
	template<class ClientContext>
	Server makeMultiplexedSSLServer(
		size_t numThreads,
		ClientCallback<ClientContext> readCallback,
		ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) {
		return Server(new ServerImpl<ClientContext>(
			new ServerSocket(new OpenSSLSocket(socketFactory().createSocketImpl())),
			new ConnectionPool(new MultiplexedConnectionPoolImpl(numThreads)),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback));
	};
	template<class ClientContext>
	Server makeThreadedSSLServer(
		ClientCallback<ClientContext> readCallback,
		ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &cd, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &cd, std::istream &inp, std::ostream &outp){},
		ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &cd, std::istream &inp, std::ostream &outp){}) {
		return Server(new ServerImpl<ClientContext>(
			new ServerSocket(new OpenSSLSocket(socketFactory().createSocketImpl())),
			new ConnectionPool(new ThreadedConnectionPoolImpl()),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback));
	};
}

}
#endif /* SRC_SOCKET_SSL_OPENSSLSOCKET_H_ */
