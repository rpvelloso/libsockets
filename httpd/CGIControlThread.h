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

#ifndef CGICONTROLTHREAD_H_
#define CGICONTROLTHREAD_H_

#include <map>
#include <sys/types.h>
#include <sys/wait.h>
#include <libsockets.h>

class HTTPClientSocket;

class CGIControlThread : public AbstractThread {
public:
	CGIControlThread();
	virtual ~CGIControlThread();
    void onStart();
    void onStop();
    void execute();

    void addPID(pid_t, HTTPClientSocket *);
    HTTPClientSocket *removePID(pid_t);
protected:
    Mutex *mutex;
    map<pid_t,HTTPClientSocket *> socketmap;
};
#endif /* CGICONTROLTHREAD_H_ */
