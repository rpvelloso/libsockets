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
#ifndef TFTPLOG_H_
#define TFTPLOG_H_

#include <cstdarg>
#include "tftpclientsocket.h"

#ifdef WIN32 // under windows, this functions are thread-safe
	#define gmtime_r(i,j) memcpy(j,gmtime(i),sizeof(struct tm))
#endif

class tFTPClientSocket;

class tFTPLog : public tLogger {
public:
	tFTPLog();
	~tFTPLog();
	void Log(const char *fmt, ...);
	void Log(tFTPClientSocket *, const char *fmt, ...);
	int Open();
	void Close();

protected:
	tMutex *mutex;
};

#endif
