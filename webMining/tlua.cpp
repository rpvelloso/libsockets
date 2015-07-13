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

static int lua_api_unloadDOMTree(lua_State *L) {
	int nargs = lua_gettop(L);
	tlua *ctx;

	if (nargs == 1) {
		lua_getglobal(L,"context");
		ctx = (tlua *)lua_touserdata(L,-1);
		lua_pop(L,1);

		if (lua_islightuserdata(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-1);
			if (checkDOM(L,dom)) {
				ctx->removeDOM(dom);
				delete dom;
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

static int lua_api_SRDExtract(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 1) {
		if (lua_islightuserdata(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-1);
			if (checkDOM(L,dom)) {
				dom->tpsf.SRDE(dom->getBody(),true);
			}
		}
	}
	return 0;
}

static int lua_api_MDRExtract(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 2) {
		if (lua_islightuserdata(L,-2) && lua_isnumber(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-2);
			if (checkDOM(L,dom)) {
				float st = lua_tonumber(L,-1);
				dom->mdr.mineDataRecords(dom->getBody(),10,st,1);
			}
		}
	}
	return 0;
}

static int lua_api_getRegionCount(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 2) {
		tDOM *dom = (tDOM *)lua_touserdata(L,-2);
		string methodstr = lua_tostring(L,-1);
		tExtractInterface *method;

		if ((methodstr == "drde") || (methodstr == "srde"))
			method = &(dom->tpsf);
		else
			method = &(dom->mdr);
		lua_pushnumber(L,method->getRegionCount());
		return 1;
	}
	return 0;
}

static int lua_api_getDataRegion(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 3) {
		if (lua_islightuserdata(L,-3) && lua_isstring(L,-2) && lua_isnumber(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-3);
			string methodstr = lua_tostring(L,-2);

			if (checkDOM(L,dom) && ((methodstr == "mdr") || (methodstr == "drde") || (methodstr == "srde"))) {
				int dtr_no = lua_tonumber(L,-1);
				int rec_no=0;
				vector<tNode *> rec;
				tExtractInterface *method;
				tTPSRegion *tpsreg;

				if (methodstr == "mdr")
					method = &(dom->mdr);
				else
					method = &(dom->tpsf);

				// main table returned
				lua_createtable(L,0,0);

				// tps data of this data region
				if ((methodstr == "drde") || (methodstr == "srde")) {
					tpsreg = dom->tpsf.getRegion(dtr_no);
					if (tpsreg && tpsreg->tps.size()) {
						lua_pushstring(L,"tps");
						lua_createtable(L,tpsreg->tps.size(),0);
						for (size_t i=0;i<tpsreg->tps.size();i++) {
							lua_pushnumber(L,i+1);
							lua_pushnumber(L,tpsreg->tps[i]);
							lua_settable(L,-3);
						}
						lua_settable(L,-3);

						lua_pushstring(L,"a");
						lua_pushnumber(L,tpsreg->lc.a);
						lua_settable(L,-3);
						lua_pushstring(L,"b");
						lua_pushnumber(L,tpsreg->lc.b);
						lua_settable(L,-3);
						lua_pushstring(L,"e");
						lua_pushnumber(L,tpsreg->lc.e);
						lua_settable(L,-3);
						lua_pushstring(L,"d");
						lua_pushnumber(L,tpsreg->stddev);
						lua_settable(L,-3);
						lua_pushstring(L,"content");
						lua_pushboolean(L,tpsreg->content);
						lua_settable(L,-3);
						lua_pushstring(L,"pos");
						lua_pushnumber(L,tpsreg->pos);
						lua_settable(L,-3);
					}
				}

				// no. of cols of table "records"
				lua_pushstring(L,"cols");
				lua_pushnumber(L,method->getRecord(dtr_no,0).size());
				lua_settable(L,-3);

				// bidimensional table containing the records
				lua_pushstring(L,"records");
				lua_createtable(L,0,0);
				while ((rec = method->getRecord(dtr_no,rec_no++)).size()) {
					lua_pushnumber(L,rec_no);
					lua_createtable(L,rec.size(),0);
					for (size_t i=0;i<rec.size();i++) {
						lua_pushnumber(L,i+1);
						if (rec[i]) {
							if ((rec[i]->getTagName() == "img") || (rec[i]->getTagName() == "a"))
								lua_pushstring(L,rec[i]->toString().c_str());
							else if (rec[i]->getType() == 2)
								lua_pushstring(L,rec[i]->getText().c_str());
							else
								lua_pushstring(L,rec[i]->toString().c_str());
								//lua_pushlightuserdata(L,rec[i]);
						} else
							lua_pushlightuserdata(L,rec[i]);
						lua_settable(L,-3);
					}
					lua_settable(L,-3);
				}
				lua_settable(L,-3);

				// no. of rows of table "records"
				lua_pushstring(L,"rows");
				lua_pushnumber(L,rec_no-1);
				lua_settable(L,-3);

				return 1;
			}
		}
	}
	return 0;
}

static int lua_api_DOMTPS(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 1) {
		if (lua_islightuserdata(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-1);

			if (checkDOM(L,dom)) {
				wstring tps = dom->tpsf.getTagPathSequence(-1);

				lua_createtable(L,tps.size(),0);
				for (size_t i=0;i<tps.size();i++) {
					lua_pushnumber(L,i+1);
					lua_pushnumber(L,tps[i]);
					lua_settable(L,-3);
				}
				return 1;
			}
		}
	}
	return 0;
}

static int lua_api_printDOM(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 2) {
		if (lua_islightuserdata(L,-2) && lua_isboolean(L,-1)) {
			tDOM *dom = (tDOM *)lua_touserdata(L,-2);
			bool verbose = lua_toboolean(L,-1);

			if (checkDOM(L,dom)) {
				dom->setVerbose(verbose);
				dom->printDOM();
			}
		}
	}
	return 0;
}

static int lua_api_nodeToString(lua_State *L) {
	int nargs = lua_gettop(L);

	if (nargs == 1) {
		if (lua_islightuserdata(L,-1)) {
			tNode *node = (tNode *)lua_touserdata(L,-1);

			lua_pushstring(L,node->toString().c_str());
			return 1;
		}
	}
	return 0;
}

}

void tlua::insertDOM(tDOM *d) {
	if (d)
		dom_set.insert(d);
}

void tlua::removeDOM(tDOM *d) {
	if (checkDOM(d)) {
		dom_set.erase(d);
		delete d;
	}
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
		LUA_SET_GLOBAL_CFUNC(L,"unloadDOMTree",lua_api_unloadDOMTree);
		LUA_SET_GLOBAL_CFUNC(L,"DRDExtract",lua_api_DRDExtract);
		LUA_SET_GLOBAL_CFUNC(L,"SRDExtract",lua_api_SRDExtract);
		LUA_SET_GLOBAL_CFUNC(L,"MDRExtract",lua_api_MDRExtract);
		LUA_SET_GLOBAL_CFUNC(L,"getDataRegion",lua_api_getDataRegion);
		LUA_SET_GLOBAL_CFUNC(L,"getRegionCount",lua_api_getRegionCount);
		LUA_SET_GLOBAL_CFUNC(L,"DOMTPS",lua_api_DOMTPS);
		LUA_SET_GLOBAL_CFUNC(L,"printDOM",lua_api_printDOM);
		LUA_SET_GLOBAL_CFUNC(L,"nodeToString",lua_api_nodeToString);
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

