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


//TODO: sobrescrever demais funcoes de saida 'Lua' e funcoes de entrada

#include <lua.hpp>
#include "LuaControlThread.h"
#include "HTTPServerSocket.h"

static int lua_cgi_print(lua_State *L) {
	HTTPClientSocket *client;
	int nargs = lua_gettop(L);

	lua_getglobal(L,"CLIENT_SOCKET");
	client = (HTTPClientSocket *)lua_touserdata(L,-1);
	lua_pop(L,1);
	if (!client) return 0;

	for (int i=1;i<=nargs;i++) {
		int len=lua_objlen(L,i);
		char *buf=NULL;

		if (lua_isstring(L,i) || lua_isnumber(L,i))
			buf=(char *)lua_tostring(L,i);
		else if (lua_isuserdata(L,i))
			buf=(char *)lua_touserdata(L,i);

		if (buf && (len > 0))
			client->getCGIOutputBuffer()->write(buf,len);
	}
	return 0;
}

LuaControlThread::LuaControlThread(HTTPClientSocket *c) : AbstractThread() {
	client = c;
}

LuaControlThread::~LuaControlThread() {

}

void LuaControlThread::onStart() {};

void LuaControlThread::onStop() {};

void LuaControlThread::execute() {
	size_t i=-1,j=ENV_VAR_COUNT+1;
	lua_State *L = luaL_newstate();

	luaL_openlibs(L);

	int s = luaL_loadfile(L, client->scriptFileName.c_str());

	if (!s) {
		LUA_SET_GLOBAL_LUDATA(L,"CLIENT_SOCKET",client);
		LUA_SET_GLOBAL_CFUNC(L,"print",lua_cgi_print);

		if (client->query[0] == '?') client->query.erase(0,1); // remove char '?'
		while ((i=client->query.find('&',++i))!=string::npos) j++;
		i = 0;
		string q = client->query;
		while (!q.empty()) {
			string st=HTTPClientSocket::stringTok(q,"&");
			string vname=HTTPClientSocket::stringTok(st,"=");
			string vvalue=st;

			LUA_SET_GLOBAL_STRING(L,vname.c_str(),vvalue);
		}

		LUA_SET_GLOBAL_NUMBER(L,"CONTENT_LENGTH",client->contentLength);
		LUA_SET_GLOBAL_STRING(L,"HTTP_REFERER",client->referer);
		LUA_SET_GLOBAL_STRING(L,"HTTP_HOST",client->host);
		LUA_SET_GLOBAL_STRING(L,"HTTP_USER_AGENT",client->userAgent);
		LUA_SET_GLOBAL_STRING(L,"HTTP_COOKIE",client->cookie);
		LUA_SET_GLOBAL_STRING(L,"SERVER_SOFTWARE",string("HTTPD"));
		LUA_SET_GLOBAL_STRING(L,"SERVER_NAME",client->host);
		LUA_SET_GLOBAL_STRING(L,"SERVER_ADDR",client->serverSocket->getIPAddress());
		LUA_SET_GLOBAL_NUMBER(L,"SERVER_PORT",client->serverSocket->getPort());
		LUA_SET_GLOBAL_STRING(L,"REMOTE_ADDR",client->getIPAddress());
		LUA_SET_GLOBAL_STRING(L,"DOCUMENT_ROOT",client->documentRoot);
		LUA_SET_GLOBAL_STRING(L,"SCRIPT_FILENAME",client->scriptFileName);
		LUA_SET_GLOBAL_NUMBER(L,"REMOTE_PORT",client->getPort());
		LUA_SET_GLOBAL_STRING(L,"GATEWAY_INTERFACE",string("CGI/1.1"));
		LUA_SET_GLOBAL_STRING(L,"SERVER_PROTOCOL",client->httpVersion);
		LUA_SET_GLOBAL_STRING(L,"REQUEST_METHOD",client->method);
		LUA_SET_GLOBAL_STRING(L,"QUERY_STRING",client->query);
		LUA_SET_GLOBAL_STRING(L,"REQUEST_URI",client->requestURI);
		LUA_SET_GLOBAL_STRING(L,"SCRIPT_NAME",client->scriptName);
		if (client->sslContext) {
			LUA_SET_GLOBAL_STRING(L,"HTTPS",string("on"));
			LUA_SET_GLOBAL_STRING(L,"REQUEST_SCHEME",string("https"));
		}
		if (!(client->boundary.empty())) {
			LUA_SET_GLOBAL_STRING(L,"CONTENT_TYPE",(client->contentType + "; boundary=" + client->boundary));
		} else {
			LUA_SET_GLOBAL_STRING(L,"CONTENT_TYPE",client->contentType);
		}

		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	} else {
		client->log("(.) Lua error: %s\n",lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	lua_close(L);
	client->onCGIEnd();
}
