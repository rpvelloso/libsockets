/*
 * Util.h
 *
 *  Created on: 4 de jun de 2017
 *      Author: Benutzer
 */

#ifndef WIN_DEFS_H_
#define WIN_DEFS_H_

#include <winsock2.h>
/*#include <windows.h>
#include <winsock.h>
#include <ws2tcpip.h>
*/
typedef SOCKET SocketFDType;
const SocketFDType InvalidSocketFD = INVALID_SOCKET;

extern int winSockInit();
extern void winSockCleanup();

#endif /* WIN_DEFS_H_ */