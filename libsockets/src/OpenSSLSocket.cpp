/*
 * OpenSSLSocket.cpp
 *
 *  Created on: 4 de jun de 2017
 *      Author: Benutzer
 */

#include "defs.h"
#include "OpenSSL.h"
#include "OpenSSLSocket.h"
#include "SocketFactory.h"
#include <iostream>

namespace socks {

OpenSSLSocket::OpenSSLSocket(SocketImpl* impl) : SocketImpl(), impl(impl),
		sslContext(nullptr, FreeSSLContext()),
		sslHandler(nullptr,FreeSSLHandler()) {
}

/*
 * this constructor creates an already connected SSL socket. It should be called by the
 * server when accepting a connection.
 * SSL_CTX *sslContext is owned by the server socket and should not be managed/freed.
 */

OpenSSLSocket::OpenSSLSocket(SocketFDType fd, SocketImpl *impl, SSL_CTX *sslContext)  : SocketImpl(fd), impl(impl),
		sslContext(nullptr, FreeSSLContext()),
		sslHandler(SSL_new(sslContext),FreeSSLHandler()) {

	SSL_set_fd(sslHandler.get(), (int)getFD());
	SSL_accept(sslHandler.get());
	SSL_set_mode(sslHandler.get(),
			SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER|
			SSL_MODE_ENABLE_PARTIAL_WRITE|
			SSL_MODE_RELEASE_BUFFERS|
			SSL_MODE_AUTO_RETRY);
};

OpenSSLSocket::~OpenSSLSocket() {
}

int OpenSSLSocket::receiveData(void* buf, size_t len) {
	auto ret = SSL_read(sslHandler.get(), buf, len);
	if (ret <= 0) {
		auto error = SSL_get_error(sslHandler.get(), ret);
		switch (error) {
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_X509_LOOKUP:
		case SSL_ERROR_WANT_ACCEPT:
		case SSL_ERROR_WANT_CONNECT:
			return 0;
		default:
			return -1;
		}
	}
	return ret;
}

int OpenSSLSocket::sendData(const void* buf, size_t len) {
	auto ret = SSL_write(sslHandler.get(), buf, len);
	if (ret <= 0) {
		auto error = SSL_get_error(sslHandler.get(), ret);
		switch (error) {
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_X509_LOOKUP:
		case SSL_ERROR_WANT_ACCEPT:
		case SSL_ERROR_WANT_CONNECT:
			return 0;
		default:
			return -1;
		}
	}
	return ret;
}

int OpenSSLSocket::connectTo(const std::string& host, const std::string& port) {
	int ret;
	if ((ret = impl->connectTo(host, port)) == 0) {
		sslContext.reset(SSL_CTX_new(ClientMethod()));
		if (!sslContext)
			return -1;

		sslHandler.reset(SSL_new(sslContext.get()));
		if (!sslHandler)
			return -1;

		if (SSL_set_fd(sslHandler.get(), (int)getFD()) != 1)
			return -1;

		if (SSL_connect(sslHandler.get()) != 1)
			return -1;

		SSL_set_mode(sslHandler.get(),
				SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER|
				SSL_MODE_ENABLE_PARTIAL_WRITE|
				SSL_MODE_RELEASE_BUFFERS|
				SSL_MODE_AUTO_RETRY);
	}
	return ret;
}

void OpenSSLSocket::disconnect() {
	if (sslHandler) {
		SSL_shutdown(sslHandler.get());
		sslHandler.reset(nullptr);
	}

	if (sslContext)
		sslContext.reset(nullptr);

	impl->disconnect();
}

int OpenSSLSocket::bindSocket(const std::string& bindAddr,
		const std::string& port) {
	return impl->bindSocket(bindAddr, port);
}

int OpenSSLSocket::listenForConnections(const std::string& bindAddr,
		const std::string& port) {
	int ret;

	if ((ret = impl->listenForConnections(bindAddr, port)) == 0) {
		sslContext.reset(SSL_CTX_new(ServerMethod()));
		if (!sslContext)
			return -1;

		if (SSL_CTX_use_certificate_file(
				sslContext.get(),
				openSSL.getSSLCertificateFile().c_str(),
				SSL_FILETYPE_PEM) != 1)
			return -1;

		if (SSL_CTX_use_PrivateKey_file(
				sslContext.get(),
				openSSL.getSSLKeyFile().c_str(),
				SSL_FILETYPE_PEM) != 1)
			return -1;

		if (SSL_CTX_check_private_key(sslContext.get()) != 1)
			throw std::runtime_error("SSL_CTX_check_private_key()");
	}

	return ret;
}

std::unique_ptr<SocketImpl> OpenSSLSocket::acceptConnection() {
	std::unique_ptr<SocketImpl> ret(new OpenSSLSocket(impl->getFD(), impl->acceptConnection().release(), sslContext.get()));
	return std::move(ret);
}

int OpenSSLSocket::setNonBlockingIO(bool status) {
	return impl->setNonBlockingIO(status);
}

int OpenSSLSocket::reuseAddress() {
	return impl->reuseAddress();
}

std::string OpenSSLSocket::getPort() {
	return impl->getPort();
}

size_t OpenSSLSocket::getSendBufferSize() {
	return impl->getSendBufferSize();
}

size_t OpenSSLSocket::getReceiveBufferSize() {
	return impl->getReceiveBufferSize();
}

SocketStateType OpenSSLSocket::getSocketState() {
	return impl->getSocketState();
}

void OpenSSLSocket::setSocketState(SocketStateType socketState) {
	impl->setSocketState(socketState);
}

SocketFDType OpenSSLSocket::getFD() {
	return impl->getFD();
}

}
