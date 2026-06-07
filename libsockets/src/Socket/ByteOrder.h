/** Copyright 2026 Diego in Velloso's lib and GNU GPL my balls. */
#ifndef LIBSOCKETS_BYTE_ORDER_H_
#define LIBSOCKETS_BYTE_ORDER_H_

#include <cstdint>

namespace socks {

uint64_t toNetworkByteOrder(uint64_t value);
uint32_t toNetworkByteOrder(uint32_t value);
uint16_t toNetworkByteOrder(uint16_t value);

uint64_t toHostByteOrder(uint64_t value);
uint32_t toHostByteOrder(uint32_t value);
uint16_t toHostByteOrder(uint16_t value);

}

#endif
