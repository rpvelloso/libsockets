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

static int lua_cgi_print(lua_State *L) {
	HTTPClientSocket *client;
	int nargs = lua_gettop(L);

	lua_getglobal(L,"CLIENT_SOCKET");
	client = (HTTPClientSocket *)lua_touserdata(L,lua_gettop(L));
	lua_pop(L,1);
	if (!client) return 0;

	for (int i=1;i<=nargs;i++) {
		const char *cstr=lua_tostring(L,i);
		string arg;

		if (cstr) arg=cstr;
		else arg="(null)";
		*(client->getCGIOutputBuffer()) << arg;
	}
	return 0;
}

LuaControlThread::LuaControlThread(HTTPClientSocket *c) : AbstractThread() {
	client = c;
}

virtual LuaControlThread::~LuaControlThread() {

}

void LuaControlThread::onStart() {};

void LuaControlThread::onStop() {};

void LuaControlThread::execute() {
	size_t i=-1,j=ENV_VAR_COUNT+1;
	lua_State *L = luaL_newstate();

	luaL_openlibs(L);

	int s = luaL_loadfile(L, client->scriptFileName.c_str());

	if (!s) {
		lua_pushlightuserdata(L,client);
		lua_setglobal(L,"CLIENT_SOCKET");
		lua_pushcfunction(L,lua_cgi_print);
		lua_setglobal(L,"print");

		if (client->query[0] == '?') client->query.erase(0,1); // remove char '?'
		while ((i=client->query.find('&',++i))!=string::npos) j++;
		i = 0;
		string q = client->query;
		while (!q.empty()) {
			string st=HTTPClientSocket::stringTok(q,"&");
			string vname=HTTPClientSocket::stringTok(st,"=");
			string vvalue=st;

			lua_pushstring(L,vvalue.c_str());
			lua_setglobal(L,vname.c_str());
		}

		lua_pushnumber(L,client->contentLength);
		lua_setglobal(L,"CONTENT_LENGTH");
		lua_pushstring(L,client->referer.c_str());
		lua_setglobal(L,"HTTP_REFERER");
		lua_pushstring(L,client->host.c_str());
		lua_setglobal(L,"HTTP_HOST");
		lua_pushstring(L,client->userAgent.c_str());
		lua_setglobal(L,"HTTP_USER_AGENT");
		lua_pushstring(L,client->cookie.c_str());
		lua_setglobal(L,"HTTP_COOKIE");
		lua_pushstring(L,"HTTPD");
		lua_setglobal(L,"SERVER_SOFTWARE");
		lua_pushstring(L,client->host.c_str());
		lua_setglobal(L,"SERVER_NAME");
		lua_pushstring(L,client->serverSocket->getIPAddress().c_str());
		lua_setglobal(L,"SERVER_ADDR");
		lua_pushnumber(L,client->serverSocket->getPort());
		lua_setglobal(L,"SERVER_PORT");
		lua_pushstring(L,client->getIPAddress().c_str());
		lua_setglobal(L,"REMOTE_ADDR");
		lua_pushstring(L,client->documentRoot.c_str());
		lua_setglobal(L,"DOCUMENT_ROOT");
		lua_pushstring(L,client->scriptFileName.c_str());
		lua_setglobal(L,"SCRIPT_FILENAME");
		lua_pushnumber(L,client->getPort());
		lua_setglobal(L,"REMOTE_PORT");
		lua_pushstring(L,"CGI/1.1");
		lua_setglobal(L,"GATEWAY_INTERFACE");
		lua_pushstring(L,client->httpVersion.c_str());
		lua_setglobal(L,"SERVER_PROTOCOL");
		lua_pushstring(L,client->method.c_str());
		lua_setglobal(L,"REQUEST_METHOD");
		lua_pushstring(L,client->query.c_str());
		lua_setglobal(L,"QUERY_STRING");
		lua_pushstring(L,client->requestURI.c_str());
		lua_setglobal(L,"REQUEST_URI");
		lua_pushstring(L,client->scriptName.c_str());
		lua_setglobal(L,"SCRIPT_NAME");
		if (client->sslContext) {
			lua_pushstring(L,"on");
			lua_setglobal(L,"HTTPS");
			lua_pushstring(L,"https");
			lua_setglobal(L,"REQUEST_SCHEME");
		}
		if (!(client->boundary.empty())) {
			lua_pushstring(L,(client->contentType + "; boundary=" + client->boundary).c_str());
			lua_setglobal(L,"CONTENT_TYPE");
		} else {
			lua_pushstring(L,client->contentType.c_str());
			lua_setglobal(L,"CONTENT_TYPE");
		}

		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	} else {
		client->log("(.) Lua error: %s\n",lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	lua_close(L);
	client->onCGIEnd();
}
