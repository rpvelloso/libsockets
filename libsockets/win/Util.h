/*
 * Util.h
 *
 *  Created on: 4 de jun de 2017
 *      Author: Benutzer
 */

#ifndef WIN_UTIL_H_
#define WIN_UTIL_H_

#include "WindowsSocket.h"

SOCKET getFD(SocketImpl &socket) {
	return getFDTemplate<WindowsSocket, SOCKET>(socket);
}

#endif /* WIN_UTIL_H_ */
