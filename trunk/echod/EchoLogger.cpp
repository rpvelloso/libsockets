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

#include <cstdio>
#include <cstdarg>
#include "EchoLogger.h"

EchoLogger::EchoLogger(string fn) : Object() {
	fileName = fn;
	mutex = new Mutex();
}

EchoLogger::~EchoLogger() {
	closeLog();
	delete mutex;
}

void EchoLogger::LOG(const char* fmt, ...) {
	va_list arglist;

	if (logFile.is_open()) {
		va_start(arglist,fmt);
		log(fmt,arglist);
		va_end(arglist);
	}
}

void EchoLogger::log(const char* fmt, va_list arglist) {
	char logstr[BUFSIZ];

	mutex->lock();
	if (logFile.is_open()) {
		vsprintf(logstr,fmt,arglist);
		logFile << logstr;
		logFile.flush();
	}
	mutex->unlock();
}

int EchoLogger::openLog() {
	logFile.open(fileName.c_str());
	return logFile.fail();
}

void EchoLogger::closeLog() {
	if (logFile.is_open()) logFile.close();
}
