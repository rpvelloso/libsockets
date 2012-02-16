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
#ifndef TLOGGER_H_
#define TLOGGER_H_

#ifdef DEVEL_ENV
	#include "tobject.h"
#else
	#include <libsockets/tobject.h>
#endif

enum tLogStatus {
	tLogClosed,
 	tLogOpened
};

class tLogger : public tObject {
public:
	tLogger();
	virtual ~tLogger();
	virtual void Log(const char *fmt,...) = 0;
	virtual int Open() = 0;
	virtual void Close() = 0;
	tLogStatus GetStatus();

protected:
	tLogStatus log_status;
};

#endif /* TLOGGER_H_ */
