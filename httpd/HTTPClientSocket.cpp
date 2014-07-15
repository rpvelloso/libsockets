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

//TODO: redirecionar CGIOutput antes do termino da execucao
//TODO: fazer autenticacao com crypt() e arquivo .htaccess

#include <cstdarg>
#include <cstring>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string>
#include "HTTPClientSocket.h"
#include "HTTPServerSocket.h"

#define REPLY_403 \
	"<HEAD><TITLE>Forbidden</TITLE></HEAD>" CRLF \
	"<H1>Forbidden</H1> You don\'t have permission to access " + scriptName + " on this server."

#define REPLY_404 \
	"<HEAD><TITLE>Not Found</TITLE></HEAD>" CRLF \
	"<H1>Not Found</H1> The requested object " + scriptName + " does not exist on this server."

#define REPLY_500 \
	"<HTML>" CRLF \
	"<HEAD><TITLE>Internal Server Error</TITLE></HEAD>" CRLF \
	"<BODY><H1>Internal Server Error</H1>The server encountered an unexpected"\
	" condition which prevented it from fulfilling the request.</BODY>" CRLF \
	"</HTML>"

#define REPLY_501 \
	"<HTML>" CRLF \
	"<HEAD><TITLE>Not Implemented</TITLE></HEAD>" CRLF \
	"<BODY><H1>Not Implemented</H1>This server does not implement the requested method.</BODY>" CRLF \
	"</HTML>"

static string weekDays[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static string months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

string time2str(time_t t) {
	struct tm tt;
	stringstream dtStr;

	gmtime_r(&t,&tt);
	dtStr << weekDays[tt.tm_wday] << ", " << tt.tm_mday << " " << months[tt.tm_mon] << " " << (1900+tt.tm_year) << " "
			<< tt.tm_hour << ":" << tt.tm_min << ":" << tt.tm_sec << " GMT";
	// data & time format: Mon, 31 Dec 1900 23:59:59 GMT
	return dtStr.str();
}

static string mime[][2] = {
		{".txt","text/plain"},
		{".htm.html","text/html"},
		{".gfi.jpg.jpeg.bmp.png.xpm.xbm","image/"},
		{".mp3.wav","audio/"},
		{".mpeg.avi.mp4.mkv.mpg.asf.flv","video/"},
		{".php.php3.phps","application/php"},
		{".css","text/css"},
		{"", ""}
		};

string mimeType(string f) {
	size_t p,p2;
	string ret,ext="";

	p = f.find_last_of(".");
	if (p != string::npos) {
		p2 = f.find_last_of('/');
		if ((p2 != string::npos) && (p>p2)) {
			ext = f.substr(p+1,f.length()-p);
			lowerCase(ext);
		}
	}
	if (ext.empty()) return "application/octet-stream";
	else {
		for (int i=0;!mime[i][0].empty();i++) {
			if (mime[i][0].find(ext)!=string::npos) {
				ret = mime[i][1];
				if (ret[ret.length()-1] == '/') ret = ret + ext;
				return ret;
			}
		}
	}
	return ("application/"+ext);
}

HTTPClientSocket::HTTPClientSocket() : AbstractMultiplexedClientSocket() {
	outputBuffer = new stringstream;
	requestState = HTTP_RECEIVE_HEADER;
	headerLineNo = 0;
	contentLength = 0;
	method = requestURI = scriptFileName = scriptName = query = httpVersion =	host =
	userAgent = contentType = boundary = referer = cookie = connection = "";
	saveOutputBuffer = NULL;
	documentRoot = "";
	serverSocket = NULL;
	CGIPID = -1;
	logger = NULL;
}

HTTPClientSocket::HTTPClientSocket(int fd, sockaddr_in *sin, SSL_CTX *ctx=NULL) : AbstractMultiplexedClientSocket(fd,sin,ctx) {
	outputBuffer = new stringstream;
	requestState = HTTP_RECEIVE_HEADER;
	headerLineNo = 0;
	contentLength = 0;
	method = requestURI = scriptFileName = scriptName = query = httpVersion = host =
	userAgent = contentType = boundary = referer = cookie = connection = "";
	saveOutputBuffer = NULL;
	documentRoot = "";
	serverSocket = NULL;
	CGIPID = -1;
	logger = NULL;
}

HTTPClientSocket::~HTTPClientSocket() {
	if (CGIPID != -1) kill(CGIPID,SIGKILL);
	while (CGIPID != -1);
	restoreOutputBuffer();
	delete outputBuffer;
	if (CGIOutput.is_open()) CGIOutput.tmp_close();
	if (CGIInput.is_open()) CGIInput.tmp_close();
	if (file.is_open()) file.close();
}

void HTTPClientSocket::beforeSend(void *buf, size_t &size) {
}

void HTTPClientSocket::onSend(void *buf, size_t size) {
	if (outputBuffer->rdbuf()->in_avail() == 0) {
		if (CGIPID == -1) {
			if (requestState == HTTP_PROCESS_REQUEST) {
				if (saveOutputBuffer == NULL) {
					if (file.is_open()) setOutputBuffer(&file);
					else if (CGIOutput.is_open()) setOutputBuffer(&CGIOutput);
					else requestState = HTTP_REQUEST_ENDED;
				} else {
					restoreOutputBuffer();
					requestState = HTTP_REQUEST_ENDED;
				}
			}

			if (requestState == HTTP_REQUEST_ENDED) {
				if (connection != "keep-alive") closeSocket();
				else requestState = HTTP_RECEIVE_HEADER;
			}
		/*} else {
			setOutputBuffer(&CGIOutput);
			commitBuffer();*/
		}
	}
}

void HTTPClientSocket::onReceive(void *buf, size_t size) {
	if (requestState == HTTP_RECEIVE_BODY) CGIInput.write((char *)buf,size);
	else inputBuffer.write((char *)buf,size);
	processInput();
}

void HTTPClientSocket::onConnect() {
	log("(+) Client connected from %s:%d.\n",getIPAddress().c_str(),getPort());
}

void HTTPClientSocket::onDisconnect() {
	log("(-) Client disconnected from %s:%d.\n",getIPAddress().c_str(),getPort());
}

void HTTPClientSocket::onCGIEnd() {
	/*if (CGIInput.is_open()) CGIInput.tmp_close();
	setOutputBuffer(&CGIOutput);
	commitBuffer();
	CGIPID = -1;*/

	if (CGIInput.is_open()) CGIInput.tmp_close();
	CGIOutput.seekg(0);
	if (CGIOutput.rdbuf()->in_avail() > 0) {
		string l,L;

		getline(CGIOutput,l);
		L = l.substr(0,11);
		upperCase(L);
		if (L.substr(0,8) == "STATUS: ") {
			sendBufferedData(httpVersion + L.substr(7,4) + CRLF);
			sendBufferedData("Connection: " + connection + CRLF);
		} else if (L.substr(0,5) != "HTTP/") {
			sendBufferedData(httpVersion + " 200 OK" + CRLF);
			sendBufferedData("Connection: " + connection + CRLF);
			CGIOutput.seekg(0);
		} else sendBufferedData(l);
	} else {
		CGIOutput.tmp_close();
		requestState = HTTP_REQUEST_ENDED;
	}
	CGIPID = -1;
	commitBuffer();
}

string HTTPClientSocket::stringTok(string &s, string d) {
	string tok="";
	unsigned int i=0;

	while ((d.find(s[i++])!=string::npos) && (i<s.size()));
	for (--i;i<s.size();i++) {
		if (d.find(s[i])==string::npos) tok = tok + s[i];
		else break;
	}
	while ((d.find(s[i++])!=string::npos) && (i<s.size()));
	s.erase(0,--i);
	return tok;
}

void HTTPClientSocket::log(const char* fmt, ...) {
	va_list arglist;

	if (logger) {
		va_start(arglist,fmt);
		logger->log(fmt,arglist);
		va_end(arglist);
	}
}

string HTTPClientSocket::unescapeUri(string u) {
	size_t p;
	string hex;
	char c,*s;

	while ((p=u.find('%')) != string::npos) {
		hex = "0x" + u.substr(p+1,2);
		c = strtol(hex.c_str(),&s,16);
		u.replace(p,3,1,c);
	}
	return u;
}

void HTTPClientSocket::processInput() {
	while (	(requestState == HTTP_RECEIVE_HEADER) &&
			(inputBuffer.str().substr(inputBuffer.tellg()).find(LF) != string::npos)) {
		string line;

		getline(inputBuffer,line);
		line.erase(remove(line.begin(),line.end(),CR),line.end());
		//log("%s\n",line.c_str());

		headerLineNo++;
		if (headerLineNo == 1) {
			contentLength = 0;
			host = userAgent = contentType = boundary =
			referer = cookie = connection = "";
			method = stringTok(line," ");
			scriptFileName = requestURI = stringTok(line," ");
			while (scriptFileName[0]=='/') scriptFileName.erase(0,1);
			scriptFileName = unescapeUri(documentRoot + stringTok(scriptFileName,"?"));
			scriptName = stringTok(requestURI,"?");
			query = requestURI;
			requestURI = scriptName;
			if (query.size()) {
				if (query[0]=='?') requestURI = requestURI + query;
				else requestURI = requestURI + "?" + query;
			}
			httpVersion = line;
		} else {
			string parm;
			stringstream cl;

			if (line.length() <= 1) {
				if (contentLength > 0) {
					if (CGIInput.is_open()) CGIInput.tmp_close();
					CGIInput.tmp_open();
					if (CGIInput.fail()) {
						reply(REPLY_500_INTERNAL_SERVER_ERROR);
						requestState = HTTP_REQUEST_ENDED;
					} else {
						while (inputBuffer.rdbuf()->in_avail()) {
							char buf[BUFSIZ];
							streamsize size;

							size = inputBuffer.readsome(buf,BUFSIZ);
							if (size>0) CGIInput.write(buf,size);
						}
						requestState = HTTP_RECEIVE_BODY;
					}
				} else requestState = HTTP_PROCESS_REQUEST;
			} else {
				parm = stringTok(line,": ");
				upperCase(parm);
				if (parm == "HOST") host = line;
				else if (parm == "USER-AGENT") userAgent = line;
				else if (parm == "REFERER") referer = line;
				else if (parm == "COOKIE") cookie = line;
				else if (parm == "CONNECTION") {
					connection = line;
					lowerCase(connection);
				} else if (parm == "CONTENT-TYPE") {
					contentType = stringTok(line,"; ");
					stringTok(line,"=");
					boundary = line;
				} else if (parm == "CONTENT-LENGTH") {
					cl << line;
					cl >> contentLength;
				}
			}
		}
	}

	if ((requestState == HTTP_RECEIVE_BODY) &&
		(CGIInput.tellg() == contentLength))
		requestState = HTTP_PROCESS_REQUEST
	;

	if (requestState == HTTP_PROCESS_REQUEST) {
		if (httpVersion.empty()) httpVersion = "HTTP/1.1";
		if (connection.empty()) connection = "close";
		/*log("Method.....: \'%s\'\n" \
			"URI........: \'%s\'\n" \
			"Query......: \'%s\'\n" \
			"Version....: \'%s\'\n" \
			"Host.......: \'%s\'\n" \
			"Agent......: \'%s\'\n" \
			"Length.....: \'%d\'\n" \
			"Type.......: \'%s\'\n" \
			"Boundary...: \'%s\'\n" \
			"Script.....: \'%s\'\n" \
			"Referer....: \'%s\'\n" \
			"Connection.: \'%s\'\n" \
			"Cookie.....: \'%s\'\n\n",
			method.c_str(),scriptFileName.c_str(),query.c_str(),httpVersion.c_str(),host.c_str(),
			userAgent.c_str(),contentLength,contentType.c_str(),boundary.c_str(),
			scriptName.c_str(),referer.c_str(),connection.c_str(),cookie.c_str());*/
		processRequest();
	}
}

static string idx[] = {
		"/index.php",
		"/index.htm",
		"/index.html",
		""
};

int HTTPClientSocket::checkURI() {
	struct stat st;
	string index;
	int i=0;

	if (!stat(scriptFileName.c_str(),&st)) {
		if (S_ISDIR(st.st_mode)) {
			while (!idx[i].empty()) {
				index = scriptFileName + idx[i++];
				if (!stat(index.c_str(),&st) && !S_ISDIR(st.st_mode)) {
					scriptFileName = index;
					scriptName = scriptName + idx[i-1];
					return 0;
				}
			}
		} else return 0;
	}
	return -1;
}

void HTTPClientSocket::processRequest() {
	headerLineNo = 0;
	if (scriptName != "*") {
		if (checkURI()) {
			reply(REPLY_404_NOT_FOUND);
			return;
		}
	}
	     if (method == "GET")     GET();
 	else if (method == "HEAD")    HEAD();
	else if (method == "POST")    POST();
	else if (method == "OPTIONS") OPTIONS();

	     /* unimplemented methods */
	else if (method == "PUT")     PUT();
	else if (method == "DELETE")  DEL();
	else if (method == "TRACE")   TRACE();
	else if (method == "CONNECT") CONNECT();
	else reply(REPLY_501_NOT_IMPLEMENTED);

	commitBuffer();
}

void HTTPClientSocket::GET() {
	string mt = mimeType(scriptName);

	if ((mt == "application/php") ||
		((!query.empty()) && !access(scriptFileName.c_str(),X_OK))) {

		executeCGI();

	} else {

		if (HEAD()) {
			file.open(scriptFileName.c_str(),fstream::in | fstream::binary);
			if (file.fail()) reply(REPLY_500_INTERNAL_SERVER_ERROR);
		}

	}
}

bool HTTPClientSocket::HEAD() {
	struct stat st;
	stringstream len;

	stat(scriptFileName.c_str(),&st);
	if (access(scriptFileName.c_str(),R_OK)) {
		reply(REPLY_403_FORBIDDEN);
		return false;
	}
	len << st.st_size;
	contentLength = st.st_size;
	sendBufferedData(httpVersion + " 200 OK" + CRLF);
	replyServer();
	replyDate();
	sendBufferedData("Content-length: " + len.str() + CRLF);
	sendBufferedData("Content-type: " + mimeType(scriptName) + CRLF);
	sendBufferedData("Last-modified: " + time2str(st.st_mtime) + CRLF);
	sendBufferedData("Accept-ranges: bytes" CRLF);
	sendBufferedData(CRLF);
	return true;
}

void HTTPClientSocket::POST() {
	GET();
}

void HTTPClientSocket::OPTIONS() {
	sendBufferedData(httpVersion + " 200 OK" + CRLF);
	replyServer();
	replyDate();
	sendBufferedData("Content-length: 0" CRLF);
	sendBufferedData("Allow: HEAD, GET, POST, OPTIONS" CRLF);
	requestState = HTTP_REQUEST_ENDED;
}

void HTTPClientSocket::PUT() {
	reply(REPLY_501_NOT_IMPLEMENTED);
}

void HTTPClientSocket::DEL() {
	reply(REPLY_501_NOT_IMPLEMENTED);
}

void HTTPClientSocket::TRACE() {
	reply(REPLY_501_NOT_IMPLEMENTED);
}

void HTTPClientSocket::CONNECT() {
	reply(REPLY_501_NOT_IMPLEMENTED);
}

void HTTPClientSocket::reply(HTTPReply r) {
	string html;
	stringstream len;

	log("(.) Replied %d to client %s:%d.\n",r,getIPAddress().c_str(),getPort());
	switch (r) {
		case REPLY_403_FORBIDDEN:
			html = REPLY_403;
			sendBufferedData(httpVersion + " 403 Forbidden." CRLF);
			break;
		case REPLY_404_NOT_FOUND:
			html = REPLY_404;
			sendBufferedData(httpVersion + " 404 Not Found." CRLF);
			break;
		case REPLY_500_INTERNAL_SERVER_ERROR:
			html = REPLY_500;
			sendBufferedData(httpVersion + " 500 Internal Server Error." CRLF);
			break;
		case REPLY_501_NOT_IMPLEMENTED:
			html = REPLY_501;
			sendBufferedData(httpVersion + " 501 Not Implemented." CRLF);
			break;
		default: return;
	}
	replyServer();
	replyDate();
	len << html.length();
	sendBufferedData("Content-length: " + len.str() + CRLF);
	sendBufferedData("Content-type: text/html" CRLF);
	sendBufferedData("Connection: " + connection + CRLF);
	sendBufferedData(CRLF);
	sendBufferedData(html);
	requestState = HTTP_REQUEST_ENDED;
}
void HTTPClientSocket::replyServer() {
	sendBufferedData("Server: httpd-libsockets-devel." CRLF);
}

void HTTPClientSocket::replyDate() {
	sendBufferedData("Date: " + time2str(time(NULL)) + CRLF);
}

HTTPServerSocket* HTTPClientSocket::getServerSocket() {
	return serverSocket;
}

void HTTPClientSocket::setServerSocket(HTTPServerSocket* s) {
	setLogger(s->getLogger());
	serverSocket = s;
}

string HTTPClientSocket::getDocumentRoot() {
	return documentRoot;
}

void HTTPClientSocket::setDocumentRoot(string d) {
	documentRoot = d;
}

#define ENV_VAR_COUNT 21
#define PHP_BIN "/usr/bin/php-cgi"

void HTTPClientSocket::executeCGI() {
	char **envp=NULL,*argv[3] = { NULL, NULL, NULL };
	size_t i=-1,j=ENV_VAR_COUNT+1;
	string q,mt;
	stringstream sstr;

	connection = "close";

	if (CGIOutput.is_open()) CGIOutput.tmp_close();
	CGIOutput.tmp_open();

	if (CGIOutput.fail()) {
		reply(REPLY_500_INTERNAL_SERVER_ERROR);
		return;
	}

	if (!(CGIPID=fork())) {
		if (query[0] == '?') query.erase(0,1); // remove char '?'
		while ((i=query.find('&',++i))!=string::npos) j++;
		i = 0;
		q = query;
		envp = new char *[j];
		while (!q.empty()) envp[i++] = strdup(stringTok(q,"&").c_str());

		sstr << "CONTENT_LENGTH=" << contentLength;
		envp[i++] = strdup(sstr.str().c_str());
		envp[i++] = strdup(("HTTP_REFERER=" + referer).c_str());
		envp[i++] = strdup(("HTTP_HOST=" + host).c_str());
		envp[i++] = strdup(("HTTP_USER_AGENT=" + userAgent).c_str());
		envp[i++] = strdup(("HTTP_COOKIE=" + cookie).c_str());
		envp[i++] = strdup("SERVER_SOFTWARE=HTTPD");
		envp[i++] = strdup(("SERVER_NAME=" + host).c_str());
		envp[i++] = strdup(("SERVER_ADDR=" + serverSocket->getIPAddress()).c_str());
		sstr.str(""); sstr << "SERVER_PORT=" << serverSocket->getPort();
		envp[i++] = strdup(sstr.str().c_str());
		envp[i++] = strdup(("REMOTE_ADDR=" + getIPAddress()).c_str());
		envp[i++] = strdup(("DOCUMENT_ROOT=" + documentRoot).c_str());
		envp[i++] = strdup(("SCRIPT_FILENAME=" + scriptFileName).c_str());
		sstr.str(""); sstr << "REMOTE_PORT=" << this->getPort();
		envp[i++] = strdup(sstr.str().c_str());
		envp[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
		envp[i++] = strdup(("SERVER_PROTOCOL=" + httpVersion).c_str());
		envp[i++] = strdup(("REQUEST_METHOD=" + method).c_str());
		envp[i++] = strdup(("QUERY_STRING=" + query).c_str());
		envp[i++] = strdup(("REQUEST_URI=" + requestURI).c_str());
		envp[i++] = strdup(("SCRIPT_NAME=" + scriptName).c_str());
		if (sslContext) envp[i++] = strdup("HTTPS=on");
		if (!boundary.empty())
			envp[i++] = strdup(("CONTENT_TYPE=" + contentType + "; boundary=" + boundary).c_str());
		else
			envp[i++] = strdup(("CONTENT_TYPE=" + contentType).c_str());
		envp[i] = NULL;

		mt = mimeType(scriptName);

		if (mt == "application/php") {
			argv[0] = strdup(PHP_BIN);
			argv[1] = strdup(scriptFileName.c_str());
		} else argv[0] = strdup(scriptFileName.c_str());

		if ((CGIInput.is_open()) && (contentLength > 0) && (method == "POST")) {
			CGIInput.flush();
			CGIInput.seekg(0);
			dup2(((fdbuf *)CGIInput.rdbuf())->fd(),fileno(stdin));
		} else fclose(stdin);
		dup2(((fdbuf *)CGIOutput.rdbuf())->fd(),fileno(stdout));
		//fclose(stderr);
		execve(argv[0],argv,envp);

		char errstr[80];
		strerror_r(errno,errstr,80);
		log("(.) execve(%s %s) error:\n",argv[0],argv[1]?argv[1]:"",errstr);
		reply(REPLY_500_INTERNAL_SERVER_ERROR);
		exit(-1);
	} else if (CGIPID == -1) {
		log("(.) CGI process not started.\n");
		requestState = HTTP_REQUEST_ENDED;
		if (CGIOutput.is_open()) CGIOutput.tmp_close();
		if (CGIInput.is_open()) CGIInput.tmp_close();
		reply(REPLY_500_INTERNAL_SERVER_ERROR);
	} else {
		//sendBufferedData(httpVersion + " 200 OK" + CRLF);
		CGIOutput.tmp_reopen(fstream::in);
		serverSocket->getCGIControlThread()->addPID(CGIPID,this);
	}
}

void HTTPClientSocket::setOutputBuffer(iostream* b) {
	if (saveOutputBuffer == NULL) {
		saveOutputBuffer = b;
		swap(outputBuffer,saveOutputBuffer);
		outputBuffer->flush();
	}
}

void HTTPClientSocket::restoreOutputBuffer() {
	if (saveOutputBuffer) {
		swap(saveOutputBuffer,outputBuffer);
		if (((fstream *)saveOutputBuffer)->is_open()) {
			if (saveOutputBuffer == &file)
				((fstream *)saveOutputBuffer)->close();
			else
				((tmpfstream *)saveOutputBuffer)->tmp_close();
		}
		saveOutputBuffer = NULL;
	}
}

LoggerInterface* HTTPClientSocket::getLogger() {
	return logger;
}

void HTTPClientSocket::setLogger(LoggerInterface* l) {
	logger = l;
}
