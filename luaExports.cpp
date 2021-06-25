#include "luaExports.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <Windows.h>
#include "mainMenu.h"
#include "resourceWindow.h"

extern "C" {
#include <lua/lstate.h>
#include <lua/lapi.h>

	int luaB_print(lua_State* L);
	TValue* index2adr(lua_State* L, int idx);
}

void loadFunction(lua_State *L, lua_CFunction func, const char* name) {
	lua_pushcfunction(L, func);
	lua_setfield(L, LUA_GLOBALSINDEX, name);
}

int lua_strToHash(lua_State* L);
int lua_intToToken(lua_State* L);
int lua_tokenToInt(lua_State* L);
int lua_getRegistry(lua_State* L);
int lua_getEnv(lua_State* L);
int lua_getKeyState(lua_State* L);
int lua_addOptionStatus(lua_State* L);
int lua_getOptionStatus(lua_State* L);
int lua_start_res(lua_State* L);
int lua_push_res(lua_State* L);
int lua_end_res(lua_State* L);
int lua_get_res(lua_State* L);

static int test_f(lua_State *L) {
	int d = lua_tonumber(L, 1);  /* get argument */
	//lua_pushnumber(L, sin(d));  /* push result */
	char buff[256];
	sprintf(buff, "%d", d);
	MessageBoxA(NULL, buff, buff, MB_OK);
	return 0;  /* number of results */

	//return 1;
}

void loadLuaExports(lua_State* L) {
	loadFunction(L, lua_strToHash, "strToHash");
	loadFunction(L, lua_strToHash, "hash");

	loadFunction(L, lua_intToToken, "intToToken");
	loadFunction(L, lua_intToToken, "to_token");

	loadFunction(L, lua_tokenToInt, "tokenToInt");
	loadFunction(L, lua_tokenToInt, "from_token");

	loadFunction(L, lua_getRegistry, "getRegistry");
	loadFunction(L, lua_getEnv, "getEnv");

	loadFunction(L, lua_getKeyState, "getKeyState");
	loadFunction(L, lua_addOptionStatus, "addOption");
	loadFunction(L, lua_getOptionStatus, "getOption");

	loadFunction(L, test_f, "test_f");

	loadFunction(L, luaB_print, "print");
	loadFunction(L, luaB_print, "pprint");

	loadFunction(L, lua_start_res, "start_res");
	loadFunction(L, lua_push_res, "push_res");
	loadFunction(L, lua_end_res, "end_res");
	loadFunction(L, lua_get_res, "get_res");
}

static unsigned int __fastcall hash_to_int(char* val, unsigned int len) {
	uint32_t hash = 0xF42E1C3E;
	uint32_t prime = 0x1000193;

	for (int i = 0; i < len; ++i) {
		uint8_t value = val[i];
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;
}

static int hextoint(char input) {
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	throw std::invalid_argument("Invalid input string");
}

static void hextobin(const char* src, char* target) {
	while (*src && src[1]) {
		*(target++) = hextoint(*src) * 16 + hextoint(src[1]);
		src += 2;
	}
}

static int lua_strToHash(lua_State* L) {
	const char* str = luaL_checkstring(L, 1);
	unsigned int hash = hash_to_int((char*)str, strlen(str));

	TValue i_o;
	*((unsigned int*)(&(i_o.value))) = hash;
	i_o.tt = LUA_TBOOLEAN;
	L->top++;
	luaA_pushobject(L, &i_o);

	return 1;
}

static int lua_intToToken(lua_State* L) {
	TValue i_o;

	const char* in = lua_tostring(L, 1);
	unsigned int val = std::stoul(in, nullptr, 16);
	*((unsigned int*)(&(i_o.value))) = val;
	i_o.tt = LUA_TBOOLEAN;
	L->top++;
	luaA_pushobject(L, &i_o);
	return 1;
}

static int lua_tokenToInt(lua_State* L) {
	const TValue* o = index2adr(L, 1);
	char buff[128];

	sprintf(buff, "0x%X", (o->value));
	lua_pushstring(L, buff);

	return 1;
}

static int lua_getRegistry(lua_State* L) {
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	return 1;
}

static int lua_getEnv(lua_State* L) {
	L->top->tt = L->env.tt;
	L->top->value = L->env.value;
	L->top++;
	return 1;
}

static int lua_getKeyState(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	lua_pushboolean(L, GetKeyState(str[0]) & 0x8000);
	return 1;
}

static int lua_addOptionStatus(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	GUI::addOption(str);
	return 0;
}

static int lua_getOptionStatus(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	lua_pushboolean(L, GUI::getOptionStatus(str));
	return 1;
}

static int lua_start_res(lua_State* L) {
	GUI::Resource::clearResourcesCache();
	return 0;
}

static int lua_push_res(lua_State* L) {
	GUI::Resource::pushResource(lua_tostring(L, -1));
	return 0;
}

static int lua_end_res(lua_State* L) {
	GUI::Resource::updateResourcesFromCache();
	return 0;
}

static int lua_get_res(lua_State* L) {
	lua_pushnumber(L, GUI::Resource::getItemCount());
	lua_pushstring(L, GUI::Resource::getItemName().c_str());
	return 2;
}