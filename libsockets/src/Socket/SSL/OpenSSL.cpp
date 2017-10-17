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

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <pthread.h>

#include "Socket/SSL/OpenSSL.h"

namespace socks {

/*
 * Code extracted from OpenSSL th-lock.c example
 * TODO: wrap all this in C++ resource managing classes (malloc's and all)
 */

OpenSSL openSSL;

/*-
 * usage:
 * CRYPTO_thread_setup();
 * application code
 * CRYPTO_thread_cleanup();
 */

static pthread_mutex_t *lock_cs;
static long *lock_count;

void pthreads_locking_callback(int mode, int type, const char *file, int line)
{
    if (mode & CRYPTO_LOCK) {
        pthread_mutex_lock(&(lock_cs[type]));
        lock_count[type]++;
    } else {
        pthread_mutex_unlock(&(lock_cs[type]));
    }
}

unsigned long pthreads_thread_id(void)
{
    unsigned long ret;

    ret = (unsigned long)pthread_self();
    return (ret);
}

void CRYPTO_thread_setup(void)
{
    int i;

    lock_cs = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
    lock_count = (long *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(long));
    if (!lock_cs || !lock_count) {
        /* Nothing we can do about this...void function! */
        if (lock_cs)
            OPENSSL_free(lock_cs);
        if (lock_count)
            OPENSSL_free(lock_count);
        return;
    }
    for (i = 0; i < CRYPTO_num_locks(); i++) {
        lock_count[i] = 0;
        pthread_mutex_init(&(lock_cs[i]), NULL);
    }

    CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
    CRYPTO_set_locking_callback((void (*)(int, int, const char *, int))pthreads_locking_callback);
}

void CRYPTO_thread_cleanup(void)
{
    int i;

    CRYPTO_set_locking_callback(NULL);
    for (i = 0; i < CRYPTO_num_locks(); i++) {
        pthread_mutex_destroy(&(lock_cs[i]));
    }
    OPENSSL_free(lock_cs);
    OPENSSL_free(lock_count);
}

}
