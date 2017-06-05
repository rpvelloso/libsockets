/*
 * OpenSSLSocket.cpp
 *
 *  Created on: 4 de jun de 2017
 *      Author: Benutzer
 */

#include "OpenSSLSocket.h"
#include "SocketFactory.h"
#include "Util.h"

#include <iostream>

int SSLInit() {
	SSL_load_error_strings();
	return SSL_library_init();
}

OpenSSLSocket::OpenSSLSocket(SocketImpl* impl) : SocketImpl(), impl(impl),
		sslContext(nullptr, FreeSSLContext()),
		sslHandler(nullptr,FreeSSLHandler()) {
}

/*
 * this constructor creates an already connected SSL socket. It should be called by the
 * server when accepting a connection.
 * SSL_CTX *sslContext is owned by the server socket and should not be managed/freed.
 */

OpenSSLSocket::OpenSSLSocket(SocketImpl *impl, SSL_CTX *sslContext)  : SocketImpl(), impl(impl),
		sslContext(nullptr, FreeSSLContext()),
		sslHandler(nullptr,FreeSSLHandler()) {

	sslHandler.reset(SSL_new(sslContext));
	SSL_set_fd(sslHandler.get(), getFD(*this->impl));
	SSL_accept(sslHandler.get());
};

OpenSSLSocket::~OpenSSLSocket() {
}

int OpenSSLSocket::receiveData(void* buf, size_t len) {
	return SSL_read(sslHandler.get(), buf, len);
}

int OpenSSLSocket::sendData(const void* buf, size_t len) {
	return SSL_write(sslHandler.get(), buf, len);
}

int OpenSSLSocket::connectTo(const std::string& host, const std::string& port) {
	int ret;
	if ((ret = impl->connectTo(host, port)) == 0) {
		sslContext.reset(SSL_CTX_new(TLSv1_2_client_method()));
		if (sslContext.get() == nullptr)
			return -1;

		sslHandler.reset(SSL_new(sslContext.get()));
		if (sslHandler.get() == nullptr)
			return -1;

		if (SSL_set_fd(sslHandler.get(), getFD(*impl)) != 1)
			return -1;

		if (SSL_connect(sslHandler.get()) != 1)
			return -1;
	}
	return ret;
}

void OpenSSLSocket::disconnect() {
	return impl->disconnect();
	if (sslHandler.get() != nullptr)
		sslHandler.reset(nullptr);

	if (sslContext.get() != nullptr)
		sslContext.reset(nullptr);
}

int OpenSSLSocket::listenForConnections(const std::string& bindAddr,
		const std::string& port) {
	int ret;

	if ((ret = impl->listenForConnections(bindAddr, port)) == 0) {

		sslContext.reset(SSL_CTX_new(TLSv1_2_server_method()));
		if (sslContext.get() == nullptr)
			return -1;

		if ((ret = SSL_CTX_use_certificate_file(sslContext.get(),socketFactory.getSSLCertificateFile().c_str(),SSL_FILETYPE_PEM)) <= 0) {
			ERR_print_errors_fp(stderr);
			return ret;
		}
		if ((ret = SSL_CTX_use_PrivateKey_file(sslContext.get(),socketFactory.getSSLKeyFile().c_str(),SSL_FILETYPE_PEM)) <= 0) {
			ERR_print_errors_fp(stderr);
			return ret;
		}
		if (!SSL_CTX_check_private_key(sslContext.get())) {
			throw std::runtime_error("SSL_CTX_check_private_key()");
		}
	}
	return ret;
}

std::unique_ptr<SocketImpl> OpenSSLSocket::acceptConnection() {
	std::unique_ptr<SocketImpl> ret(new OpenSSLSocket(impl->acceptConnection().release(), sslContext.get()));
	return std::move(ret);
}

int OpenSSLSocket::setNonBlockingIO(bool status) {
	return setNonBlockingIO(status);
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
