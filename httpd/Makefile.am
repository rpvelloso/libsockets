AUTOMAKE_OPTIONS = foreign
CFLAGS = -Wall -ggdb
AM_LDFLAGS = -lssl -lcrypto -lpthread -lsockets -llua

bin_PROGRAMS = httpd
httpd_SOURCES = CGIControlThread.cpp HTTPLogger.cpp HTTPThread.h CGIControlThread.h HTTPLogger.h LuaControlThread.cpp HTTPClientSocket.cpp HTTPServerSocket.cpp LuaControlThread.h HTTPClientSocket.h HTTPServerSocket.h httpd.cpp HTTPThread.cpp

