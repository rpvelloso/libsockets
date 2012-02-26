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
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <libsockets/libsockets.h>
#include "thttpthread.h"
#include "thttpclientsocket.h"

#define ERR_STR_LEN 100

#ifdef WIN32 // under windows, this functions are thread-safe
	#define gmtime_r(i,j) memcpy(j,gmtime(i),sizeof(struct tm))
	#define strerror_r(i,j,k) strerror(i)
#endif

#define CRLF "\r\n"

static string weekdays[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static string months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

string time2str(time_t t) {
	struct tm tt;
	stringstream dtstr;

	gmtime_r(&t,&tt);
	dtstr << weekdays[tt.tm_wday] << ", " << tt.tm_mday << " " << months[tt.tm_mon] << " " << (1900+tt.tm_year) << " "
			<< tt.tm_hour << ":" << tt.tm_min << ":" << tt.tm_sec << " GMT";
	// data & time format: Mon, 31 Dec 1900 23:59:59 GMT
	return dtstr.str();
}

// TODO: add more MIME types
static string textExt = ".txt";
static string htmlExt = ".htm.html";
static string imageExt = ".gfi.jpg.jpeg.bmp.png.xpm.xbm";
static string audioExt = ".mp3.wav";
static string videoExt = ".mpeg.avi.mp4.mkv.mpg.asf.flv";

string ext2type(string f) {
	size_t p,p2;
	string ret,ext="";

	p = f.find_last_of(".",f.length());
	if (p != string::npos) {
		p2 = f.find_last_of("/",f.length());
		if ((p2 != string::npos) && (p>p2)) {
			ext = f.substr(p+1,f.length()-p);
			lower_case(ext);
		}
	}
	if (ext == "") ret = "application/octet-stream";
	else if (textExt.find(ext,0)!=string::npos) ret = "text/plain";
	else if (htmlExt.find(ext,0)!=string::npos) ret = "text/html";
	else if (imageExt.find(ext,0)!=string::npos) ret = "image/"+ext;
	else if (audioExt.find(ext,0)!=string::npos) ret = "audio/"+ext;
	else if (videoExt.find(ext,0)!=string::npos) ret = "video/"+ext;
	else ret = "application/"+ext;

	return ret;
}

string unescapeUri(string uri) {
	size_t p;
	string hex;
	char c,*s;

	while ((p=uri.find('%',0)) != string::npos) {
		hex = "0x" + uri.substr(p+1,2);
		c = strtol(hex.c_str(),&s,16);
		uri.replace(p,3,1,c);
	}
	return uri;
}

tHTTPClientSocket::tHTTPClientSocket(int fd, sockaddr_in *sin) : tClientSocket(fd, sin) {
	log = NULL;
	msgOverflow = 0;
	hdrPos = 0;
	lineLength = 0;
	reqState = tHTTPReceiveHeader;
	httpBody = NULL;
}

tHTTPClientSocket::~tHTTPClientSocket() {
	if (httpBody) free(httpBody);
	Close();
}

void tHTTPClientSocket::SetLog(tHTTPLog *l) {
	log = l;
}

void tHTTPClientSocket::OnSend(void *buf, size_t *size) {
	string *s = (string *)buf;

	log->Log("sent to %s: %s",GetHostName(),s->c_str());
}

void tHTTPClientSocket::OnReceive(void *buf, size_t size) {
	size_t i=0;

	for (;(i<size) && (reqState == tHTTPReceiveHeader);i++) {
		if (((char *)buf)[i] == ENDL) {
			if (!lineLength) {
				httpHeader[hdrPos] = 0x00;
				hdrPos = 0;
				if (msgOverflow) {
					msgOverflow = 0;
					LOG("HTTP header buffer overflow.\n");
				} else if (strlen(httpHeader) > 0) ProcessHTTPHeader();
			}
			lineLength = 0;
		} else {
			if (hdrPos >= HTTP_HDR_LEN) {
				hdrPos = 0;
				msgOverflow = 1;
			}
			if (((char *)buf)[i] != '\r') {
				httpHeader[hdrPos++] = ((char *)buf)[i];
				lineLength++;
			} else httpHeader[hdrPos++] = '\n';
		}
	}
	if (reqState == tHTTPReceiveBody) {
		size_t cpyLen = (size-i)>contentLength?contentLength:(size-i);
		memcpy(&(((char *)httpBody)[bodyPos]),&(((char *)buf)[i]),cpyLen);
		bodyPos += cpyLen;
		if (bodyPos == contentLength) ProcessHTTPBody();
	}
	if (reqState == tHTTPProcessRequest) ProcessHTTPRequest();
}

void tHTTPClientSocket::OnConnect() {
}

void tHTTPClientSocket::OnDisconnect() {
	log->Log("connection to %s closed.\n",GetHostName());
}

void tHTTPClientSocket::ProcessHTTPHeader() {
	string line, lineAux, parm, msg = httpHeader;
	int i=0;
	stringstream cl;
	string request;

	// reset header
	host = "";
	method = "";
	uri = "";
	query = "";
	httpVersion = "";
	userAgent = "";
	contentType = "";
	boundary = "";
	contentLength = 0;

	do { // TODO: parse header params better
		line = stringtok(&msg,"\n\r"); i++;
		lineAux = line;
		if (i == 1) { // request line
			method = stringtok(&line," ");
			request = unescapeUri(stringtok(&line," "));
			uri = stringtok(&request,"?");
			query = request;
			httpVersion = line;
		} else { // header lines
			parm = stringtok(&line,": ");
			upper_case(parm);
			if (parm == "HOST") {
				host = line;
			} else if (parm == "USER-AGENT") {
				userAgent = line;
			} else if (parm == "CONTENT-TYPE") {
				contentType = stringtok(&line,"; ");
				stringtok(&line,"=");
				boundary = line;
			} else if (parm == "CONTENT-LENGTH") {
				cl << line;
				cl >> contentLength;
			}
		}
	} while (lineAux != "");

	cout << "Method..: \'" << method << "\'" << endl
	<< "URI.....: \'" << uri << "\'" << endl
	<< "Query...: \'" << query << "\'" << endl
	<< "Version.: \'" << httpVersion << "\'" << endl
	<< "Host....: \'" << host << "\'" << endl
	<< "Agent...: \'" << userAgent << "\'" << endl
	<< "Length..: \'" << contentLength << "\'" << endl
	<< "Type....: \'" << contentType << "\'" << endl
	<< "Boundary: \'" << boundary << "\'" << endl;

	if (contentLength > 0) {
		if (httpBody) {
			free(httpBody);
			httpBody = NULL;
		}
		httpBody = malloc(contentLength+1);
		bodyPos = 0;
		((char *)httpBody)[contentLength] = 0x00;
		reqState = tHTTPReceiveBody;
	} else reqState = tHTTPProcessRequest;
}

void tHTTPClientSocket::ProcessHTTPBody() {
	cout << "Body....: " << endl;
	cout << "\'" << (char *)httpBody << "\'" << endl;

	reqState = tHTTPProcessRequest;
}

void tHTTPClientSocket::ProcessHTTPRequest() {
	     if (method == "GET")     GET();
 	else if (method == "HEAD")    HEAD();
	else if (method == "POST")    POST();
	else if (method == "OPTIONS") OPTIONS();
	else if (method == "PUT")     PUT();
	else if (method == "DELETE")  DEL();
	else if (method == "TRACE")   TRACE();
	else if (method == "CONNECT") CONNECT();
	else {
		Reply501();
	}
	reqState = tHTTPReceiveHeader; // goes back to initial state
}

void tHTTPClientSocket::GET()
{
	off_t offset = 0;
	char **envp=NULL,*argv[2] = { NULL, NULL };
	size_t i=0,j=1;
	pid_t f;
	int cgiRet;
	string q;
	char strerr[ERR_STR_LEN];
	stringstream len;

	if (query != "") {
		if (!(f=fork())) {
			while (i<query.length()) if (query[i++] == '&') j++;
			envp = (char **)malloc(sizeof(void *) * (j+11));
			i = 0; q = query;
			while (q != "") {
				envp[i++] = strdup(stringtok(&q,"&").c_str());
			}
			/* TODO: Add more CGI variables to the environ (REMOTE_ADDR, etc).
			 * The above was enough to run basic PHP CGI get/post
			 */
			len << "CONTENT_LENGTH=" << contentLength;
			envp[j] = strdup(len.str().c_str());
			if (boundary != "")
				envp[j+ 1] = strdup(("CONTENT_TYPE=" + contentType + "; boundary=" + boundary).c_str());
			else
				envp[j+ 1] = strdup(("CONTENT_TYPE=" + contentType).c_str());
			envp[j+ 2] = strdup(("REQUEST_METHOD=" + method).c_str());
			envp[j+ 3] = strdup(("HTTP_HOST=" + host).c_str());
			envp[j+ 4] = strdup(("HTTP_USER_AGENT=" + userAgent).c_str());
			envp[j+ 5] = strdup("GATEWAY_INTERFACE=CGI/1.1");
			envp[j+ 6] = strdup(("QUERY_STRING=" + query).c_str());
			envp[j+ 7] = strdup(("REQUEST_URI=" + uri).c_str());
			envp[j+ 8] = strdup(("SERVER_PROTOCOL=" + httpVersion).c_str());
			envp[j+ 9] = strdup(("SCRIPT_FILENAME=" + uri).c_str());
			envp[j+10] = NULL;
			argv[0] = strdup(uri.c_str());
			/* TODO: in the future change this redirection of stdout
			 * to the client socket. Instead of redirecting, use a
			 * pipe to intermediate CGI output and detect if the
			 * CGI hasn't sent HTTP response header, so the server
			 * can send/complete it */
			dup2(this->socket_fd,fileno(stdout));

			// Send POST data to CGI stdin
			if ((httpBody) && (contentLength > 0) && (method == "POST")) {
				int pp[2];

				if (!pipe(pp)) {
					write(pp[1],httpBody,contentLength);
					close(pp[1]);
					dup2(pp[0],fileno(stdin));
				} else {
					exit(-1);
				}
			} else dup2(this->socket_fd,fileno(stdin));

			execve(argv[0],argv,envp);
			LOG(strerror_r(errno,strerr,ERR_STR_LEN)); // execve() error
			exit(-1);
		} else if (f == -1) {
			LOG(strerror_r(errno,strerr,ERR_STR_LEN)); // fork() error
			Reply500();
		} else {
			waitpid(f,&cgiRet,0);
			if (cgiRet) Reply500();	// CGI has executed, but ended abnormally
			Close();
		}
	} else {
		if (!HEAD()) SendFile(uri.c_str(),&offset,contentLength);
	}
}

void tHTTPClientSocket::OPTIONS()
{
	Reply501();
}

int tHTTPClientSocket::HEAD()
{
	struct stat st;
	stringstream len;
	string index;

	if (!stat(uri.c_str(),&st)) {
		if (S_ISDIR(st.st_mode)) { // TODO: create option to list files in dir with no index.html
			index = uri + "/index.htm";
			if (stat(index.c_str(),&st)) {
				index = uri + "/index.html";
				if (stat(index.c_str(),&st)) {
					Reply404();
					return -1;
				}
			}
			uri = index;
		}
		if (access(uri.c_str(),R_OK)) {
			Reply403();
			return -1;
		}
		len << st.st_size;
		contentLength = st.st_size;
		Send(httpVersion + " 200 OK" + CRLF);
		ReplyServer();
		ReplyDate();
		Send("Content-length: " + len.str() + CRLF);
		Send("Content-type: " + ext2type(uri) + CRLF);
		if (index == uri) Send("Content-Location: " + index + CRLF);
		Send("Last-modified: " + time2str(st.st_mtime) + CRLF);
		Send("Accept-ranges: bytes" CRLF);
		Send(CRLF);
		return 0;
	} else {
		Reply404();
		return -1;
	}
}

void tHTTPClientSocket::POST()
{
	GET();
}

void tHTTPClientSocket::PUT()
{
	Reply501();
}

void tHTTPClientSocket::DEL()
{
	Reply501();
}

void tHTTPClientSocket::TRACE()
{
	Reply501();
}

void tHTTPClientSocket::CONNECT()
{
	Reply501();
}

void tHTTPClientSocket::ReplyServer()
{
	Send("Server: httpd-libsockets-devel." CRLF);
}

void tHTTPClientSocket::ReplyDate()
{
	Send("Date: " + time2str(time(NULL)) + CRLF);
}

void tHTTPClientSocket::Reply403()
{
	string html =
			"<HEAD><TITLE>Forbidden</TITLE></HEAD>" CRLF \
			"<H1>Forbidden</H1> You don\'t have permission to access " + uri + " on this server.";
	stringstream len;

	len << html.length();
	Send(httpVersion + " 403 Forbidden." + CRLF);
	ReplyServer();
	ReplyDate();
	Send("Content-length: " + len.str() + CRLF);
	Send("Content-type: text/html" CRLF);
	Send("Connection: close" CRLF);
	Send(CRLF);
	Send(html);
}

void tHTTPClientSocket::Reply404()
{
	string html =
			"<HEAD><TITLE>Not Found</TITLE></HEAD>" CRLF \
			"<H1>Not Found</H1> The requested object " + uri + " does not exist on this server.";
	stringstream len;

	len << html.length();
	Send(httpVersion + " 404 Not Found." + CRLF);
	ReplyServer();
	ReplyDate();
	Send("Content-length: " + len.str() + CRLF);
	Send("Content-type: text/html" CRLF);
	Send("Connection: close" CRLF);
	Send(CRLF);
	Send(html);
}

void tHTTPClientSocket::Reply500()
{
	string html =
			"<HTML>" CRLF \
			"<HEAD><TITLE>Internal Server Error</TITLE></HEAD>" CRLF \
			"<BODY><H1>Internal Server Error</H1>The server encountered an unexpected"\
			" condition which prevented it from fulfilling the request.</BODY>" CRLF \
			"</HTML>";
	stringstream len;

	len << html.length();
	Send(httpVersion + " 500 Internal Server Error." + CRLF);
	ReplyServer();
	ReplyDate();
	Send("Content-length: " + len.str() + CRLF);
	Send("Content-type: text/html" CRLF);
	Send("Connection: close" CRLF);
	Send(CRLF);
	Send(html);
}

void tHTTPClientSocket::Reply501()
{
	string html =
			"<HTML>" CRLF \
			"<HEAD><TITLE>Not Implemented</TITLE></HEAD>" CRLF \
			"<BODY><H1>Not Implemented</H1>This server does not implement the requested method.</BODY>" CRLF \
			"</HTML>";
	stringstream len;

	len << html.length();
	Send(httpVersion + " 501 Not Implemented." + CRLF);
	ReplyServer();
	ReplyDate();
	Send("Content-length: " + len.str() + CRLF);
	Send("Content-type: text/html" CRLF);
	Send("Connection: close" CRLF);
	Send(CRLF);
	Send(html);
}
