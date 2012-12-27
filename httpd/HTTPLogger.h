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

#ifndef HTTPLOGGER_H_
#define HTTPLOGGER_H_

#include <fstream>
#include <libsockets.h>

class HTTPLogger: public Object, public LoggerInterface {
public:
	HTTPLogger(string);
	virtual ~HTTPLogger();
	void log(const char *fmt, va_list);
	int openLog();
	void closeLog();
protected:
	fstream logFile;
	string fileName;
	Mutex *mutex;
};

#endif /* HTTPLOGGER_H_ */
