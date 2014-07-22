AUTOMAKE_OPTIONS = foreign
CFLAGS = -fPIC -Wall -ggdb
AM_LDFLAGS = -shared -lssl -lcrypto -lpthread 

lib_LTLIBRARIES = libsockets.la
libsockets_la_SOURCES = AbstractClientSocket.cpp AbstractThread.cpp AbstractClientSocket.h AbstractThread.h AbstractMultiplexedClientSocket.cpp ClientSocketMultiplexer.cpp AbstractMultiplexedClientSocket.h ClientSocketMultiplexer.h AbstractMultiplexerThread.cpp libsockets.h AbstractMultiplexerThread.h LoggerInterface.h AbstractServer.cpp MultiplexedServer.cpp AbstractServer.h MultiplexedServer.h AbstractServerSocket.cpp Mutex.cpp AbstractServerSocket.h Mutex.h AbstractSocket.cpp Object.cpp AbstractSocket.h Object.h
include_HEADERS=libsockets.h AbstractClientSocket.h ClientSocketMultiplexer.h AbstractMultiplexedClientSocket.h libsockets.h AbstractMultiplexerThread.h LoggerInterface.h AbstractServer.h MultiplexedServer.h AbstractServerSocket.h Mutex.h AbstractSocket.h Object.h AbstractThread.h

