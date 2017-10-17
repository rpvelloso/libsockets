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

#ifndef SRC_SOCKET_SSL_OPENSSL_H_
#define SRC_SOCKET_SSL_OPENSSL_H_

#include <memory>
#include <functional>
#include <iostream>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <pthread.h>

namespace socks {

extern void CRYPTO_thread_setup();
extern void CRYPTO_thread_cleanup();

class OpenSSL {
public:
	OpenSSL() {
	    SSL_load_error_strings();
	    OpenSSL_add_ssl_algorithms();
		SSL_library_init();
		CRYPTO_thread_setup();
	};

	virtual ~OpenSSL() {
		CRYPTO_thread_cleanup();
		FIPS_mode_set(0);
		ENGINE_cleanup();
		CONF_modules_unload(1);
		EVP_cleanup();
		ERR_free_strings();
	};
	/* Command to generate test files 'key.pem' and 'cert.pem':
	 * openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
	 */
	virtual std::string getSSLKeyFile() {
		return "key.pem";
	};
	virtual std::string getSSLCertificateFile() {
		return "cert.pem";
	};
};

extern OpenSSL openSSL;

}
#endif /* SRC_SOCKET_SSL_OPENSSL_H_ */
