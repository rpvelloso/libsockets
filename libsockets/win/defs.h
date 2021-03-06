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

#ifndef WIN_DEFS_H_
#define WIN_DEFS_H_

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#include <winsock2.h>

namespace socks {

typedef SOCKET SocketFDType;
const SocketFDType InvalidSocketFD = INVALID_SOCKET;

}
#endif /* WIN_DEFS_H_ */
