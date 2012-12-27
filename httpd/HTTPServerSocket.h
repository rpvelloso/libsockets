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

#ifndef HTTPSERVERSOCKET_H_
#define HTTPSERVERSOCKET_H_

#include <cstdarg>
#include <libsockets.h>
#include "HTTPClientSocket.h"
#include "HTTPLogger.h"

extern string rootDir;

class HTTPServerSocket : public AbstractServerSocket<HTTPClientSocket> {
public:
	HTTPServerSocket() : AbstractServerSocket<HTTPClientSocket>() {
		logger = new HTTPLogger("/dev/stderr");
		logger->openLog();
	};
	virtual ~HTTPServerSocket() { delete logger; };
	void onServerUp() { log("(*) Server started. Addr: %s:%d, root directory: %s\n",getIPAddress().c_str(),getPort(),rootDir.c_str()); };
    void onServerDown() { log("(*) Server ended.\n"); };
	void onClientConnect(HTTPClientSocket *c) {
		log("(+) Client connected from %s:%d.\n",c->getIPAddress().c_str(),c->getPort());
		c->setDocumentRoot(rootDir);
		c->setServerSocket(this);
	}
	LoggerInterface* getLogger() { return logger; }
	void setLogger(LoggerInterface* l) { logger = l; }
protected:
    LoggerInterface *logger;

    void log(const char *fmt, ...) {
    	va_list arglist;

    	if (logger) {
    		va_start(arglist,fmt);
    		logger->log(fmt,arglist);
    		va_end(arglist);
    	}
    }
};

#endif /* HTTPSERVERSOCKET_H_ */
