/*
 * OpenSSLSocket.h
 *
 *  Created on: 4 de jun de 2017
 *      Author: Benutzer
 */

#ifndef SRC_OPENSSLSOCKET_H_
#define SRC_OPENSSLSOCKET_H_

#define OPENSSL_NO_SHA
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "ClientSocket.h"
#include "SocketImpl.h"

int SSLInit();

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

using SSLCtxPtr = std::unique_ptr<SSL_CTX, FreeSSLContext>;
using SSLHandlerPtr = std::unique_ptr<SSL, FreeSSLHandler>;

class OpenSSLSocket: public SocketImpl {
public:
	OpenSSLSocket(SocketImpl *impl);
	OpenSSLSocket(SocketImpl *impl, SSL_CTX *sslContext);
	virtual ~OpenSSLSocket();
	virtual int receiveData(void *buf, size_t len);
	virtual int sendData(const void *buf, size_t len);
	virtual int connectTo(const std::string &host, const std::string &port);
	virtual void disconnect();
	virtual int listenForConnections(const std::string &bindAddr, const std::string &port);
	virtual std::unique_ptr<ClientSocket> acceptConnection();
	virtual int setNonBlockingIO(bool status);
	virtual int reuseAddress();
	virtual std::string getPort();
	virtual size_t getSendBufferSize();
	virtual size_t getReceiveBufferSize();
private:
	std::unique_ptr<SocketImpl> impl;
	SSLCtxPtr sslContext;
	SSLHandlerPtr sslHandler;
};

#endif /* SRC_OPENSSLSOCKET_H_ */
