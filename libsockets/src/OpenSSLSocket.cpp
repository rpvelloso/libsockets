/*
 * OpenSSLSocket.cpp
 *
 *  Created on: 4 de jun de 2017
 *      Author: Benutzer
 */

#include <defs.h>
#include "OpenSSLSocket.h"
#include "SocketFactory.h"
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
	std::cout << "HN: " << (sslHandler.get() != nullptr) << std::endl;
	std::cout << "FD: " << getFD() << " " << SSL_set_fd(sslHandler.get(), (int)getFD())  << std::endl;
	std::cout << "AC: " << SSL_accept(sslHandler.get())  << std::endl;
	std::cout << "ST: " << (int)getSocketState()  << std::endl;
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

		if (SSL_set_fd(sslHandler.get(), (int)getFD()) != 1)
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

		if (SSL_CTX_use_certificate_file(
				sslContext.get(),
				socketFactory.getSSLCertificateFile().c_str(),
				SSL_FILETYPE_PEM) != 1)
			return -1;

		if (SSL_CTX_use_PrivateKey_file(
				sslContext.get(),
				socketFactory.getSSLKeyFile().c_str(),
				SSL_FILETYPE_PEM) != 1)
			return -1;

		if (SSL_CTX_check_private_key(sslContext.get()) != 1)
			throw std::runtime_error("SSL_CTX_check_private_key()");
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

SocketStateType OpenSSLSocket::getSocketState() {
	return impl->getSocketState();
}

void OpenSSLSocket::setSocketState(SocketStateType socketState) {
	impl->setSocketState(socketState);
}

SocketFDType OpenSSLSocket::getFD() {
	return impl->getFD();
}
