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
#include <sstream>
#include <iomanip>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "ftpreply.h"
#include "tftpdatasocket.h"

tFTPDataSocket::tFTPDataSocket() : tClientSocket() {
	transferComplete = 0;
}

tFTPDataSocket::tFTPDataSocket(int fd, sockaddr_in *sin) : tClientSocket(fd,sin) {
	transferComplete = 0;
}

tFTPDataSocket::~tFTPDataSocket() {
	Close();
}

void tFTPDataSocket::setTransferComplete(int tc) {
	transferComplete = tc;
}

static const char *month[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

void tFTPDataSocket::list(string p, int list_type) {
	DIR *d;
	struct dirent *de;
	stringstream ls;
	string filepath;
	string mode;
	struct stat st;
	struct tm tm1;
#ifndef WIN32
	struct dirent *entryp;
	size_t len;
	char errstr[ERRSTR_LEN];
#endif

	if (!p.empty() && p[0] != '-') {
		p = getControlConnection()->resolvePath(p);
	} else p = getControlConnection()->getCWD();

#ifndef WIN32
	len = offsetof(struct dirent, d_name) +
          pathconf(p.c_str(), _PC_NAME_MAX) +
          1;
	entryp = (struct dirent *)malloc(len);

	if (entryp) {
#else
	if (1) {
#endif
		getControlConnection()->Send(R150);
		if ((d = opendir(p.c_str()))) {
			readdir_r(d,entryp,&de);
			while (de) {
				if (p == DIR_DELIM_STR) filepath = p + de->d_name;
				else filepath = p + DIR_DELIM_STR + de->d_name;
				if (stat(filepath.c_str(),&st) != -1) {
					gmtime_r(&st.st_mtime,&tm1);
					if (list_type == 0) {
						mode = S_ISDIR(st.st_mode)?"d":"-";
						mode = mode + ((S_IRUSR&st.st_mode)?"r":"-");
						mode = mode + ((S_IWUSR&st.st_mode)?"w":"-");
						mode = mode + ((S_IXUSR&st.st_mode)?"x":"-");
#ifdef WIN32
						mode = mode + "------";
#else
						mode = mode + ((S_IRGRP&st.st_mode)?"r":"-");
						mode = mode + ((S_IWGRP&st.st_mode)?"w":"-");
						mode = mode + ((S_IXGRP&st.st_mode)?"x":"-");
						mode = mode + ((S_IROTH&st.st_mode)?"r":"-");
						mode = mode + ((S_IWOTH&st.st_mode)?"w":"-");
						mode = mode + ((S_IXOTH&st.st_mode)?"x":"-");
#endif
						ls << mode <<" "<<setw(3)<<(unsigned int)st.st_nlink<<" user     group "
						<<setw(11)<<(unsigned int)st.st_size<<" "<<setw(3)<<month[tm1.tm_mon]<<" "
						<<setw(2)<<tm1.tm_mday<<" "<<setw(5)<<tm1.tm_year+1900<<" "<<de->d_name<<"\r\n";
					} else ls << de->d_name << "\r\n";
				} else {
					controlConnection->getLog()->log(controlConnection,"error: LIST error %s - %s\n",filepath.c_str(),strerror_r(errno,errstr,ERRSTR_LEN));
				}
				readdir_r(d,entryp,&de);
			}
			Send(ls.str().c_str());
			closedir(d);
		} else {
			controlConnection->getLog()->log(controlConnection,"error: directory access error %s - %s\n",p.c_str(),strerror_r(errno,errstr,ERRSTR_LEN));
		}
#ifndef WIN32
		free(entryp);
#endif
		transferComplete = 1;
		getControlConnection()->Send(R226);
	}
	Close();
}

void tFTPDataSocket::retrieve(string f, off_t off) {
	struct stat st;
	size_t count;

	transferComplete = 1;

	if (!stat(f.c_str(),&st)) {
		if (!S_ISDIR(st.st_mode)) {
			getControlConnection()->Send(R150);
			count = st.st_size - off;
			transferComplete = 0;
			if (sendFile(f.c_str(),&off,count)>0) {
				getControlConnection()->Send(R226);
				transferComplete = 1;
			}
		} else getControlConnection()->Send(R550_RETR);
	} else getControlConnection()->Send(R550_RETR);
	Close();
}

void tFTPDataSocket::store(string f, off_t off) {
	int fd,len;
	char storbuf[STOR_BUFLEN];

	transferComplete = 1;

	if ((fd=open(f.c_str(),O_CREAT|O_WRONLY,STOR_PERM))>0) {
		if (lseek(fd,off,SEEK_SET)!=-1) {
			getControlConnection()->Send(R150);
			while ((len=receive(storbuf,STOR_BUFLEN))>0) {
				if (write(fd,storbuf,len)!=len) {
					transferComplete = 0;
					break;
				}
			}
			if (len!=0) getControlConnection()->Send(R550_STOR);
			else getControlConnection()->Send(R226);
		} else getControlConnection()->Send(R550_STOR);
		close(fd);
	} else getControlConnection()->Send(R550_STOR);
	Close();
}

tFTPClientSocket *tFTPDataSocket::getControlConnection() {
	return controlConnection;
}

void tFTPDataSocket::setControlConnection(tFTPClientSocket *cc) {
	controlConnection = cc;
}

void tFTPDataSocket::onConnect() {}
void tFTPDataSocket::onSend(void *, size_t *) {}
void tFTPDataSocket::onReceive(void *, size_t) {}
void tFTPDataSocket::onDisconnect() {
	if (!transferComplete) getControlConnection()->Send(R426);
}
