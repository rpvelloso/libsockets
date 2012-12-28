/*
 * CGIControlThread.h
 *
 *  Created on: Dec 28, 2012
 *      Author: roberto
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
    HTTPClientSocket *removeSocket(pid_t);
    void processCGIOutput(HTTPClientSocket *);
protected:
    Mutex *mutex;
    map<pid_t,HTTPClientSocket *> socketmap;
};
#endif /* CGICONTROLTHREAD_H_ */
