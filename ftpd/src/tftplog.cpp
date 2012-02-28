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
#include "tftplog.h"

tFTPLog::tFTPLog() : tLogger() {
	mutex = new tMutex();
}

tFTPLog::~tFTPLog() {
	delete mutex;
}

void tFTPLog::log(const char *fmt, ...) {
	va_list arglist;
	time_t t = time(NULL);
	struct tm tm1;

	gmtime_r(&t,&tm1);

	if (logStatus != tLogClosed) {
		mutex->lock();
		fprintf(stderr,"%04d.%02d.%02d.%02d:%02d:%02d ",tm1.tm_year+1900,tm1.tm_mon+1,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
		va_start(arglist,fmt);
		vfprintf(stderr,fmt,arglist);
		va_end(arglist);
		mutex->unlock();
	}
}

void tFTPLog::log(tFTPClientSocket *c, const char *fmt, ...) {
	va_list arglist;
	time_t t = time(NULL);
	struct tm tm1;

	gmtime_r(&t,&tm1);

	if (logStatus != tLogClosed) {
		mutex->lock();
		fprintf(stderr,"%04d.%02d.%02d.%02d:%02d:%02d ",tm1.tm_year+1900,tm1.tm_mon+1,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
		if (c) fprintf(stderr,"%s:%d %s - ",c->getIP().c_str(),c->getPort(),c->getUsername().c_str());
		va_start(arglist,fmt);
		vfprintf(stderr,fmt,arglist);
		va_end(arglist);
		mutex->unlock();
	}
}

int tFTPLog::Open() {
	logStatus = tLogOpened;
	return 0;
}

void tFTPLog::Close() {
	logStatus = tLogClosed;
}
