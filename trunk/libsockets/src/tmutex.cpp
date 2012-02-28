/*
    Copyright 2011 Roberto Panerai Velloso.

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
#include "tmutex.h"

tMutex::tMutex() {
    pthread_mutex_init(&mutex,NULL);
}

tMutex::~tMutex() {
    pthread_mutex_destroy(&mutex);
}

int tMutex::lock() {
    return pthread_mutex_lock(&mutex);
}

int tMutex::tryLock() {
    return pthread_mutex_trylock(&mutex);
}

int tMutex::unlock() {
    return pthread_mutex_unlock(&mutex);
}
