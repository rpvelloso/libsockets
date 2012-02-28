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
#ifndef TFTPCLIENTSOCKET_H_
#define TFTPCLIENTSOCKET_H_

#include <string>
#include <libsockets/libsockets.h>
#include "tftplog.h"
#include "tftppassivesocket.h"
#include "tftpserver.h"

#define CMD_BUFLEN 1024

#define MKD_PERM 0750

#define CMD_DELIM '\n'
#define DIR_DELIM_CHAR '/'
#define DIR_DELIM_STR "/"
#define STR_DELIM " "
#define PORT_DELIM ","
#define CMD_DELIM_STR "\0\r\n"

#define LOG(...) log->log(this, __VA_ARGS__)

#ifdef WIN32 // under windows, this functions are thread-safe
	#define strerror_r(i,j,k) strerror(i)
#endif

class tFTPLog;
class tFTPPassiveSocket;
class tFTPServer;

class tFTPClientSocket : public tClientSocket {
public:
	tFTPClientSocket(int, sockaddr_in *);
    ~tFTPClientSocket();
    void setLog(tFTPLog *);
    tFTPLog *getLog();
    void processCommand();
    string getCWD();
    string getUsername();
    string resolvePath(string);
    void setOwner(tFTPServer *o);

	void QUIT();
	void USER(string);
	void PASS(string);
	void MODE(string);
	void STRU(string);
	void TYPE(string);
	void CWD(string);
	void PORT(string);
	void LIST(string, int);
	void MKD(string);
	void SIZE(string);
	void HELP();
	void NOOP();
	void SYST();
	void PWD();
	void PASV();
	void RETR(string);
	void STOR(string);
	void REST(string);
	void DELE(string);
	void RNFR(string);
	void RNTO(string);

    void onSend(void *, size_t *);
    void onReceive(void *, size_t);
    void onConnect();
    void onDisconnect();

protected:
    tFTPLog *log;
    char commandBuffer[CMD_BUFLEN];
    int cmdPos,cmdOverflow;
    string username;
    string cwd;
    string renameFrom;
    int logged;
    int passive;
    string clientIp;
    unsigned short clientPort;
    tFTPPassiveSocket *pasvSocket;
    off_t restart;
    tFTPServer *owner;
};

#endif
