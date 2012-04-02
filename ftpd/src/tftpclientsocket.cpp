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
#include <algorithm>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include "ftpreply.h"
#include "tftpclientsocket.h"

tFTPClientSocket::tFTPClientSocket(int fd, sockaddr_in *sin) : tClientSocket(fd, sin) {
	log = NULL;
	cmdPos = 0;
	cmdOverflow = 0;
	pasvSocket = NULL;
	passive = 0;
	logged = 0;
	username.clear();
	clientIp.clear();
	clientPort = 0;
	restart = 0;
	cwd = "/";
	owner = NULL;
}

tFTPClientSocket::~tFTPClientSocket() {
	Close();
	if (pasvSocket) delete pasvSocket;
}

void tFTPClientSocket::setLog(tFTPLog *l) {
	log = l;
}

tFTPLog *tFTPClientSocket::getLog() {
	return log;
}

void tFTPClientSocket::onSend(void *buf, size_t *size) {
#ifdef WIN32
/*	if ((*size) == -1) {
		string *s = ((string *)buf);

		if ((*s)[s->size()-1] == '\n') {
			s->erase(s->size()-1,s->size()-1);
			(*s) = (*s) + "\r\n";
		}
	}*/
#endif
}

void tFTPClientSocket::onReceive(void *buf, size_t size) {
	for (size_t i=0;i<size;i++) {
		if (((char *)buf)[i] == CMD_DELIM) {
			commandBuffer[cmdPos] = 0x00;
			cmdPos = 0;
			if (cmdOverflow) {
				cmdOverflow = 0;
				LOG("command buffer overflow.\n");
			} else if (strlen(commandBuffer) > 0) processCommand();
		} else {
			if (cmdPos >= CMD_BUFLEN) {
				cmdPos = 0;
				cmdOverflow = 1;
			}
			if (((char *)buf)[i] != '\r') commandBuffer[cmdPos++] = ((char *)buf)[i];
		}
	}
}

string tFTPClientSocket::getCWD() {
	return cwd;
}

string tFTPClientSocket::getUsername() {
	return username;
}

string tFTPClientSocket::resolvePath(string p) {
	string ret;
	list<string> path;
	list<string>::iterator i;
	string t,cpath,ccwd;

	if (p[0] != DIR_DELIM_CHAR) {
		ccwd = cwd;
		cpath = p;
	} else {
		ccwd = p;
		cpath = "";
	}

	t = stringTok(ccwd,DIR_DELIM_STR);
	while (!t.empty()) {
		if ((t == "..") && (path.size()>0)) path.remove(path.back());
		else if (t != ".") path.push_back(t);
		t = stringTok(ccwd,DIR_DELIM_STR);
	}

	if (!cpath.empty()) {
		t = stringTok(cpath,DIR_DELIM_STR);
		while (!t.empty()) {
			if (t == "..") path.remove(path.back());
			else if (t != ".") path.push_back(t);
			t = stringTok(cpath,DIR_DELIM_STR);
		}
	}

	ret = "";
	i=path.begin();
	for (i=path.begin();i!=path.end();i++) {
		ret = ret + DIR_DELIM_STR + (*i);
	}
	if (ret == "") ret = DIR_DELIM_STR;

	path.clear();
	return ret;
}

void tFTPClientSocket::setOwner(tFTPServer *o) {
	owner = o;
}

void tFTPClientSocket::processCommand() {
	string cmd = "";
	string parm = "";
	string buf = commandBuffer;

	LOG("%s\n",commandBuffer);
	cmd = stringTok(buf,STR_DELIM);
	parm = stringTok(buf,CMD_DELIM_STR);
	upperCase(cmd);
	if      (cmd == "QUIT") QUIT();
	else if (cmd == "USER") USER(parm);
	else if (cmd == "PASS") PASS(parm);
	else if ((cmd == "LIST") && logged) LIST(parm,0);
	else if ((cmd == "NLST") && logged) LIST(parm,1);
	else if ((cmd == "MODE") && logged) MODE(parm);
	else if ((cmd == "STRU") && logged) STRU(parm);
	else if ((cmd == "TYPE") && logged) TYPE(parm);
	else if ((cmd == "CDUP") && logged) CWD("..");
	else if ((cmd ==  "CWD") && logged) CWD(parm);
	else if ((cmd == "PORT") && logged) PORT(parm);
	else if ((cmd ==  "MKD") && logged) MKD(parm);
	else if ((cmd == "SIZE") && logged) SIZE(parm);
	else if ((cmd == "NOOP") && logged) NOOP();
	else if ((cmd == "ALLO") && logged) NOOP();
	else if ((cmd == "SYST") && logged) SYST();
	else if ((cmd == "HELP") && logged) HELP();
	else if ((cmd == "PASV") && logged) PASV();
	else if ((cmd == "RETR") && logged) RETR(parm);
	else if ((cmd == "STOR") && logged) STOR(parm);
	else if ((cmd == "REST") && logged) REST(parm);
	else if ((cmd == "RNFR") && logged) RNFR(parm);
	else if ((cmd == "RNTO") && logged) RNTO(parm);
	else if (((cmd == "DELE")  ||
	          (cmd ==  "RMD")  ||
		      (cmd == "XRMD")) && logged) DELE(parm);
	else if (((cmd ==  "PWD") ||
			  (cmd == "XPWD")) && logged) PWD();
	else Send(R500);

	if (cmd != "RNFR") renameFrom = "";
}

void tFTPClientSocket::QUIT() {
	Send(R221);
	Close();
}

void tFTPClientSocket::USER(string u) {
	if (!u.empty()) {
		username = u;
		logged = 0;
		Send(R331);
	} else Send(R501);
}

void tFTPClientSocket::PASS(string p) {
	tLogin *l;
	DIR *d;

	if (!logged && !p.empty()) {
		if (!username.empty()) {
			l = owner->authenticate(username,p);
			if (l) {
				if ((d=opendir(l->homedir.c_str()))) {
					closedir(d);
					cwd = l->homedir;
				} else cwd = "/";
				logged = 1;
				Send(R230);
			} else Send(R530);
		} else Send(R503);
	} else Send(R501);
}

void tFTPClientSocket::MODE(string m) {
	if (!m.empty()) {
		upperCase(m);
		if (m == "S") {
			Send(R200_MODE);
		} else Send(R504);
	} else Send(R501);
}

void tFTPClientSocket::TYPE(string t) {
	if (!t.empty()) {
		upperCase(t);
		if ((t == "A") || (t == "I")) {
			Send(R200_TYPE);
			Send(t.c_str());
			Send("\n");
		} else Send(R504);
	} else Send(R501);
}

void tFTPClientSocket::STRU(string s) {
	if (!s.empty()) {
		upperCase(s);
		if (s == "F") {
			Send(R200_STRU);
		} else Send(R504);
	} else Send(R501);
}

void tFTPClientSocket::CWD(string c) {
	DIR *d;

	if (!c.empty()) {
		c = resolvePath(c);
		if ((d = opendir(c.c_str()))) {
			cwd = c;
			Send(R250);
			closedir(d);
			return;
		}
		Send(R550_CWD);
	} else Send(R501);
}

void tFTPClientSocket::PORT(string a) {
	string port_char;
	int port_param[6];
	int x=0;
	stringstream ip;

	if (!a.empty()) {
		port_char = stringTok(a,PORT_DELIM);
		while (!port_char.empty() && (x < 6)) {
			port_param[x++] = atoi(port_char.c_str());
			port_char = stringTok(a,PORT_DELIM);
		}
		if (x != 6) Send(R501);
		else {
			ip << port_param[0]<<"."<<port_param[1]<<"."<<port_param[2]<<"."<<port_param[3];
			clientIp = ip.str();
			clientPort = (unsigned short)(((port_param[4] << 8) & 0xff00) | ((port_param[5]) & 0xff));
			if (pasvSocket) {
				delete pasvSocket;
				pasvSocket = NULL;
			}
			passive = 0;
			Send(R200);
		}
	} else Send(R425);
}

void tFTPClientSocket::LIST(string p, int lt) {
	tFTPDataSocket *data_socket;

	if (!passive) {
		data_socket = new tFTPDataSocket();
		if (data_socket) {
			if (data_socket->Open(clientIp.c_str(),clientPort)) Send(R425);
			else {
				data_socket->setControlConnection(this);
				data_socket->list(p,lt);
			}
			delete data_socket;
		} else Send(R425);
	} else {
		if (pasvSocket) {
			if (pasvSocket->getStatus() == tSocketListening) {
				data_socket = pasvSocket->Accept();
				delete pasvSocket;
				pasvSocket = NULL;
				if (data_socket) {
					data_socket->setControlConnection(this);
					data_socket->list(p,lt);
				} else Send(R425);
				delete data_socket;
			} else Send(R425);
		} else Send(R425);
	}
}

void tFTPClientSocket::MKD(string d) {
	if (!d.empty()) {
		d = resolvePath(d);
#ifdef WIN32
		if (mkdir(d.c_str()) != -1) Send(R257);
#else
		if (mkdir(d.c_str(),MKD_PERM) != -1) Send(R257);
#endif
		else Send(R550_MKD);
	} else Send(R501);
}

void tFTPClientSocket::SIZE(string f) {
	stringstream fsize;
	struct stat st;

	if (!f.empty()) {
		f = resolvePath(f);
		if (stat(f.c_str(),&st) != -1) {
			if (S_ISREG(st.st_mode)) {
				fsize << (unsigned int)st.st_size;
				Send(R213);
				Send(fsize.str().c_str());
				Send("\n");
			} else Send(R550_SIZE1);
		} else Send(R550_SIZE2);
	} else Send(R501);
}

void tFTPClientSocket::HELP() {
	Send(R211);
}

void tFTPClientSocket::NOOP() {
	Send(R200);
}

void tFTPClientSocket::SYST() {
	Send(R215);
}

void tFTPClientSocket::PWD() {
	Send(R257_PWD);
	Send(cwd.c_str());
	Send("\"\n");
}

void tFTPClientSocket::PASV() {
	string ip = getIP();
	unsigned short port;
	unsigned int portlo;
	unsigned int porthi;
	stringstream addr;
	size_t pos;

	passive = 0;
	if (pasvSocket) delete pasvSocket;
	pasvSocket = new tFTPPassiveSocket();
	if (!pasvSocket->Open(ip.c_str(),0)) {
		while ((pos=ip.find("."))!=string::npos) ip[pos]=',';
		port = pasvSocket->getPort();
		portlo = port & 0x00ff;
		porthi = (port >> 8) & 0x00ff;
		addr<<ip<<","<<porthi<<","<<portlo;
		Send(R227);
		Send(addr.str().c_str());
		Send(").\n");
		passive = 1;
	} else {
		delete pasvSocket;
		pasvSocket = NULL;
		Send(R425);
	}
}

void tFTPClientSocket::RETR(string f) {
	tFTPDataSocket *data_socket;
	off_t r = restart;
	restart = 0;

	if (!f.empty()) {
		f = resolvePath(f);
		if (!passive) {
			data_socket = new tFTPDataSocket();
			if (data_socket) {
				if (data_socket->Open(clientIp.c_str(),clientPort)) Send(R425);
				else {
					data_socket->setControlConnection(this);
					data_socket->retrieve(f.c_str(), r);
				}
				delete data_socket;
			} else Send(R425);
		} else {
			if (pasvSocket) {
				if (pasvSocket->getStatus() == tSocketListening) {
					data_socket = pasvSocket->Accept();
					delete pasvSocket;
					pasvSocket = NULL;
					if (data_socket) {
						data_socket->setControlConnection(this);
						data_socket->retrieve(f.c_str(), r);
					} else Send(R425);
					delete data_socket;
				} else Send(R425);
			} else Send(R425);
		}
	} else Send(R501);
}

void tFTPClientSocket::STOR(string f) {
	tFTPDataSocket *data_socket;
	off_t r = restart;
	restart = 0;

	if (!f.empty()) {
		f = resolvePath(f);
		if (!passive) {
			data_socket = new tFTPDataSocket();
			if (data_socket) {
				if (data_socket->Open(clientIp.c_str(),clientPort)) Send(R425);
				else {
					data_socket->setControlConnection(this);
					data_socket->store(f.c_str(),r);
				}
				delete data_socket;
			} else Send(R425);
		} else {
			if (pasvSocket) {
				if (pasvSocket->getStatus() == tSocketListening) {
					data_socket = pasvSocket->Accept();
					delete pasvSocket;
					pasvSocket = NULL;
					if (data_socket) {
						data_socket->setControlConnection(this);
						data_socket->store(f.c_str(),r);
					} else Send(R425);
					delete data_socket;
				} else Send(R425);
			} else Send(R425);
		}
	} else Send(R501);
}

void tFTPClientSocket::REST(string f) {
	if (!f.empty()) {
		restart = atol(f.c_str());
		Send(R350);
	} else Send(R501);
}

void tFTPClientSocket::DELE(string p) {
	if (!p.empty()) {
		p = resolvePath(p);
		if (remove(p.c_str()) != -1) Send(R250);
		else Send(R550_DELE);
	} else Send(R501);
}

void tFTPClientSocket::RNFR(string p) {
	if (!p.empty()) {
		renameFrom = resolvePath(p);
		Send(R350_RNFR);
	} else Send(R501);
}

void tFTPClientSocket::RNTO(string p) {
	if (!renameFrom.empty()) {
		if (!p.empty()) {
			p = resolvePath(p);
			if (rename(renameFrom.c_str(),p.c_str()) == -1) Send(R550_RNTO);
			else Send(R250);
		} else Send(R501);
	} else Send(R503_RNTO);
}

void tFTPClientSocket::onConnect() {
}

void tFTPClientSocket::onDisconnect() {
	LOG("connection to %s closed.\n",getHostname());
}
