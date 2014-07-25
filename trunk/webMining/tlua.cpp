/*
 * tlua.cpp
 *
 *  Created on: 25/07/2014
 *      Author: rvelloso
 */

#include <iostream>
#include <fstream>
#include <lua.hpp>
#include "tlua.h"
#include "tdom.h"

using namespace std;

extern "C" {
static int lua_api_loadDOM(lua_State *L) {
	int nargs = lua_gettop(L);
	tlua *ctx;
	tDOM *dom;
	fstream file;

	if (nargs == 1) {
		lua_getglobal(L,"CONTEXT");
		ctx = (tlua *)lua_touserdata(L,-1);
		lua_pop(L,1);

		if (lua_isstring(L,-1)) {
			file.open(lua_tostring(L,-1));
			if (file.is_open()) {
				dom = new tDOM();
				dom->scan(file);
				file.close();
				ctx->insertDOM(dom);
				lua_pushlightuserdata(L,dom);
				return 1;
			}
		}
		return 0;
	}
	return 0;
}

static int lua_api_DRDExtract(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 2) {
		if (lua_islightuserdata(L,-2) && lua_isnumber(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-2);
			float st = lua_tonumber(L,-1);
			dom->tpsf.DRDE(dom->getBody(),true,st);
		}
	}
	return 0;
}

static int lua_api_DRDEGetRecord(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 3) {
		if (lua_islightuserdata(L,-3) && lua_isnumber(L,-2) && lua_isnumber(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-3);
			int dtr_no = lua_tonumber(L,-2);
			int rec_no = lua_tonumber(L,-1);
			vector<tNode *> rec = dom->tpsf.getRecord(dtr_no,rec_no);
			cout << rec.size() << endl;
			for (size_t i=0;i<rec.size();i++) {
				if (rec[i])
					lua_pushlightuserdata(L,rec[i]);
				else
					lua_pushnil(L);
			}
			return rec.size();
		}
	}
	return 0;
}

static int lua_api_printField(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 1) {
		if (lua_islightuserdata(L,-1)) {
			tNode *n = (tNode *)lua_touserdata(L,-1);

			if ((n->getTagName() == "img") || (n->getTagName() == "a")) {
				n->printNode(4,true);
			} else if (n->getType() == 2)
				cout << n->getText();
		}
	}
	return 0;
}

}

void tlua::insertDOM(tDOM *d) {
	if (d)
		dom_set.insert(d);
}

tlua::tlua(const char *inp) {
	lua_State *L = luaL_newstate();

	luaL_openlibs(L);

	int s = luaL_loadfile(L, inp);

	if (!s) {
		LUA_SET_GLOBAL_LUDATA(L,"CONTEXT",this);
		LUA_SET_GLOBAL_CFUNC(L,"loadDOM",lua_api_loadDOM);
		LUA_SET_GLOBAL_CFUNC(L,"DRDExtract",lua_api_DRDExtract);
		LUA_SET_GLOBAL_CFUNC(L,"DRDEGetRecord",lua_api_DRDEGetRecord);
		LUA_SET_GLOBAL_CFUNC(L,"printField",lua_api_printField);
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	} else {
		cerr << "Lua error: %s\n",lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	lua_close(L);
}

tlua::~tlua() {
	for (auto i=dom_set.begin();i!=dom_set.end();i++) {
		delete (*i);
	}
	dom_set.clear();
}

