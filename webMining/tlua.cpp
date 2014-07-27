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

int checkDOM(lua_State *L, tDOM *d) {
	tlua *ctx;

	lua_getglobal(L,"context");
	ctx = (tlua *)lua_touserdata(L,-1);
	lua_pop(L,1);
	return ctx->checkDOM(d);
}

static int lua_api_loadDOMTree(lua_State *L) {
	int nargs = lua_gettop(L);
	tlua *ctx;
	tDOM *dom;
	fstream file;

	if (nargs == 1) {
		lua_getglobal(L,"context");
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
	}
	return 0;
}

static int lua_api_DRDExtract(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 2) {
		if (lua_islightuserdata(L,-2) && lua_isnumber(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-2);
			if (checkDOM(L,dom)) {
				float st = lua_tonumber(L,-1);
				dom->tpsf.DRDE(dom->getBody(),true,st);
			}
		}
	}
	return 0;
}

static int lua_api_DRDEGetDataRegion(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 2) {
		if (lua_islightuserdata(L,-2) && lua_isnumber(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-2);

			if (checkDOM(L,dom)) {
				size_t dtr_no = lua_tonumber(L,-1);
				size_t rec_no=0;
				vector<tNode *> rec;

				lua_createtable(L,0,0);
				lua_pushstring(L,"cols");
				lua_pushnumber(L,dom->tpsf.getRecord(dtr_no,0).size());
				lua_settable(L,-3);

				lua_pushstring(L,"records");
				lua_createtable(L,0,0);
				while ((rec = dom->tpsf.getRecord(dtr_no,rec_no++)).size()) {
					lua_pushnumber(L,rec_no);
					lua_createtable(L,rec.size(),0);
					for (size_t i=0;i<rec.size();i++) {
						lua_pushnumber(L,i+1);
						if (rec[i]) {
							if ((rec[i]->getTagName() == "img") || (rec[i]->getTagName() == "a"))
								lua_pushstring(L,rec[i]->toString(4,true).c_str());
							else if (rec[i]->getType() == 2)
								lua_pushstring(L,rec[i]->getText().c_str());
							else
								lua_pushlightuserdata(L,rec[i]);
						} else
							lua_pushlightuserdata(L,rec[i]);
						lua_settable(L,-3);
					}
					lua_settable(L,-3);
				}
				lua_settable(L,-3);
				lua_pushstring(L,"rows");
				lua_pushnumber(L,rec_no-1);
				lua_settable(L,-3);
				return 1;
			}
		}
	}
	return 0;
}

static int lua_api_fieldToString(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 1) {
		if (lua_islightuserdata(L,-1)) {
			tNode *n = (tNode *)lua_touserdata(L,-1);

			if (n) {
				lua_pushstring(L,n->toString(4,true).c_str());
				return 1;
			}
		}
	}
	return 0;
}

}

void tlua::insertDOM(tDOM *d) {
	if (d)
		dom_set.insert(d);
}

int tlua::checkDOM(tDOM *d) {
	return dom_set.find(d) != dom_set.end();
}

tlua::tlua(const char *inp) {
	lua_State *L = luaL_newstate();

	luaL_openlibs(L);

	int s = luaL_loadfile(L, inp);

	if (!s) {
		LUA_SET_GLOBAL_LUDATA(L,"context",this);
		LUA_SET_GLOBAL_CFUNC(L,"loadDOMTree",lua_api_loadDOMTree);
		LUA_SET_GLOBAL_CFUNC(L,"DRDExtract",lua_api_DRDExtract);
		LUA_SET_GLOBAL_CFUNC(L,"DRDEGetDataRegion",lua_api_DRDEGetDataRegion);
		LUA_SET_GLOBAL_CFUNC(L,"fieldToString",lua_api_fieldToString);
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	} else {
		cout << "Lua error: " << lua_tostring(L, -1) << endl;
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

