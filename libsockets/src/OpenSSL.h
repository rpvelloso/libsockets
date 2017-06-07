/*
 * OpenSSL.h
 *
 *  Created on: 6 de jun de 2017
 *      Author: rvelloso
 */

#ifndef OPENSSL_H_
#define OPENSSL_H_

#include <memory>
#include <functional>
#include <iostream>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <pthread.h>

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

		//TODO: should call this inside every thread
		CRYPTO_cleanup_all_ex_data();
		ERR_remove_state(0);

		ERR_free_strings();
	};
};

#endif /* OPENSSL_H_ */
