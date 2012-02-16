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
#include "techolog.h"

tEchoLog::tEchoLog() : tLogger() {
	mutex = new tMutex();
}

tEchoLog::~tEchoLog() {
	delete mutex;
}

void tEchoLog::Log(const char *fmt,...) {
	va_list arglist;

	if (log_status != tLogClosed) {
		mutex->Lock();
		va_start(arglist,fmt);
		vfprintf(stderr,fmt,arglist);
		va_end(arglist);
		mutex->Unlock();
	}
}

int tEchoLog::Open() {
	log_status = tLogOpened;
	return 0;
}

void tEchoLog::Close() {
	log_status = tLogClosed;
}
