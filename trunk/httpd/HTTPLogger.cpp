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

#include <cstdarg>
#include <ctime>
#include "HTTPLogger.h"

HTTPLogger::HTTPLogger(string fn) : Object() {
	fileName = fn;
	mutex = new Mutex();
}

HTTPLogger::~HTTPLogger() {
	closeLog();
	delete mutex;
}

void HTTPLogger::log(const char* fmt, va_list arglist) {
	time_t t=time(NULL);
	struct tm tt;
	char logstr[BUFSIZ];

	mutex->lock();
	if (logFile.is_open()) {
   		vsnprintf(logstr,BUFSIZ,fmt,arglist);
   		gmtime_r(&t,&tt);
   		logFile << "[" << tt.tm_year+1900 << "-" << tt.tm_mon+1 << "-" << tt.tm_mday << "-"
   				<< tt.tm_hour << "." << tt.tm_min << "." << tt.tm_sec << "] ";
   		logFile << logstr;
   		logFile.flush();
	}
	mutex->unlock();
}

bool HTTPLogger::openLog() {
	logFile.open(fileName.c_str());
	return logFile.fail();
}

void HTTPLogger::closeLog() {
	if (logFile.is_open()) logFile.close();
}
