/*
 * OpenSSLSocket.h
 *
 *  Created on: 4 de jun de 2017
 *      Author: Benutzer
 */

#ifndef SRC_OPENSSLSOCKET_H_
#define SRC_OPENSSLSOCKET_H_

#include <functional>
#include <openssl/ssl.h>
#include "ClientSocket.h"
#include "SocketImpl.h"

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

using SSLMethodType = std::function<decltype(TLSv1_2_server_method)>;
using SSLCtxPtr = std::unique_ptr<SSL_CTX, FreeSSLContext>;
using SSLHandlerPtr = std::unique_ptr<SSL, FreeSSLHandler>;

class OpenSSLSocket: public SocketImpl {
public:
	OpenSSLSocket(SocketImpl *impl);
	virtual ~OpenSSLSocket();
	int receiveData(void *buf, size_t len) override;
	int sendData(const void *buf, size_t len) override;
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
private:
	OpenSSLSocket(SocketFDType fd, SocketImpl *impl, SSL_CTX *sslContext);

	std::unique_ptr<SocketImpl> impl;
	SSLCtxPtr sslContext;
	SSLHandlerPtr sslHandler;
	SSLMethodType ServerMethod = TLSv1_2_server_method;
	SSLMethodType ClientMethod = TLSv1_2_client_method;
};

}
#endif /* SRC_OPENSSLSOCKET_H_ */
