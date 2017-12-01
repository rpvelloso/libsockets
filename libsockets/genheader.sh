#!/bin/bash
headers=(
	'win/defs.h'
	
	'src/Socket/SocketAddressImpl.h'
	'src/Socket/SocketAddress.h'
	'src/Socket/SocketImpl.h'
	'src/Socket/SocketState/SocketState.h'
	'src/Socket/Socket.h'
	'src/Socket/ClientSocket.h'
	'src/Socket/ServerSocket.h'
	'src/Socket/DatagramSocket.h'
	'src/Socket/SocketStream.h'
	
	'src/Socket/BufferedClientSocketInterface.h'
	'src/Socket/BufferedClientSocket.h'
	'src/ConnectionPool/ConnectionPoolImpl.h'
	'src/ConnectionPool/ConnectionPool.h'
	'src/ConnectionPool/ThreadedConnectionPoolImpl.h'
	
	'src/Multiplexer/MultiplexerImpl.h'
	'src/Multiplexer/Multiplexer.h'
	'src/ConnectionPool/MultiplexedConnectionPoolImpl.h'
	
	'src/Server/ServerImpl.h'
	'src/Server/Server.h'
	
	'src/Multiplexer/Poll.h'
	'src/Factory/SocketFactoryImpl.h'
	'src/Factory/SocketFactory.h'
	'src/Socket/SSL/OpenSSLSocket.h'
	)

rm -rf tmp.h tmp2.h libsockets.h
touch tmp.h
i=0
while [ "${headers[i]}" != "" ]
do
   cat ${headers[i]}|egrep -v '(#include|#endif|#ifdef|#ifndef|#elif|#define|#undef)' >> tmp.h
   i=$(( $i + 1 ))
done

g++ -I/local/include/ -E tmp.h -o tmp2.h

echo '#ifndef _LIBSOCKETS_H
#define _LIBSOCKETS_H

#include <string>
#include <vector>
#include <atomic>
#include <tuple>
#include <mutex>
#include <thread>
#include <utility>
#include <memory>
#include <functional>
#include <streambuf>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <openssl/ssl.h>

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
' > libsockets.h

cat tmp2.h|grep -v '# ' >> libsockets.h

echo '
#endif // #define _LIBSOCKETS_H' >> libsockets.h
rm -rf tmp.h tmp2.h
