/** Copyright 2026 Diego in Velloso's lib and GNU GPL my balls. */
#include "Socket/ByteOrder.h"

#include <arpa/inet.h>
#include <algorithm>

namespace socks {
    
uint64_t toNetworkByteOrder(uint64_t value) { 
	return htonl(1) == 1
		? value
		: (uint64_t(htonl(value & 0xFFFFFFFFUL)) << 32) | htonl(uint32_t(value >> 32));
}
uint32_t toNetworkByteOrder(uint32_t value) { return htonl(value); }
uint16_t toNetworkByteOrder(uint16_t value) { return htons(value); }

uint64_t toHostByteOrder(uint64_t value) {
	if( ntohl(1) == 1 ) return value;
	auto * bytes = reinterpret_cast<char *>(&value);
	std::swap(bytes[0],bytes[7]);
	std::swap(bytes[1],bytes[6]);
	std::swap(bytes[2],bytes[5]);
	std::swap(bytes[3],bytes[4]);
	return value;
}
uint32_t toHostByteOrder(uint32_t value) { return ntohl(value); }
uint16_t toHostByteOrder(uint16_t value) { return ntohs(value); }

}