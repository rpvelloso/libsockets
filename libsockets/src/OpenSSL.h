/*
 * OpenSSL.h
 *
 *  Created on: 6 de jun de 2017
 *      Author: rvelloso
 */

#ifndef OPENSSL_H_
#define OPENSSL_H_

#include <memory>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <pthread.h>

class OpenSSLSuper {
public:
	OpenSSLSuper() {
	    SSL_load_error_strings();
	    OpenSSL_add_ssl_algorithms();
		SSL_library_init();
	};

	virtual ~OpenSSLSuper() {
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

static pthread_mutex_t, decltype(CRYPTO_free)> lock_cs;
std::unique_ptr<long, decltype(CRYPTO_free)> lock_count;

class OpenSSL : public OpenSSLSuper {
public:
	OpenSSL() : OpenSSLSuper(),
		lock_cs(static_cast<pthread_mutex_t *>(OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t))), CRYPTO_free),
		lock_count(static_cast<long *>(OPENSSL_malloc(CRYPTO_num_locks() * sizeof(long))), CRYPTO_free) {

	    for (int i = 0; i < CRYPTO_num_locks(); i++) {
	        lock_count[i] = 0;
	        pthread_mutex_init(&(lock_cs[i]), NULL);
	    }

	    //CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
	    //CRYPTO_set_locking_callback((void (*)())pthreads_locking_callback);
	};

	virtual ~OpenSSL() {
	    CRYPTO_set_locking_callback(NULL);
	    for (int i = 0; i < CRYPTO_num_locks(); i++) {
	        pthread_mutex_destroy(&(lock_cs[i]));
	    }
	};
private:
	std::unique_ptr<pthread_mutex_t, decltype(CRYPTO_free)> lock_cs;
	std::unique_ptr<long, decltype(CRYPTO_free)> lock_count;

	/*static void pthreads_locking_callback(int mode, int type, char *file, int line)
	{
	    if (mode & CRYPTO_LOCK) {
	        pthread_mutex_lock(&(lock_cs[type]));
	        lock_count[type]++;
	    } else {
	        pthread_mutex_unlock(&(lock_cs[type]));
	    }
	}

	static unsigned long pthreads_thread_id(void)
	{
	    unsigned long ret;

	    ret = (unsigned long)pthread_self();
	    return (ret);
	}*/
};

#endif /* OPENSSL_H_ */
