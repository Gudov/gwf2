#include "luaExports.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

extern "C" {
#include <lua/lstate.h>
}

static inline void loadFunction(lua_State *L, const char *name, lua_CFunction func) {
	lua_pushcfunction(L, func);
	lua_setfield(L, LUA_GLOBALSINDEX, name);
}

void loadLuaExports(lua_State* L) {

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

int lua_to_hash(lua_State* L) {
	const char* str = luaL_checkstring(L, 1);
	unsigned int hash = hash_to_int((char*)str, strlen(str));

	TValue i_o;
	*((unsigned int*)(&(i_o.value))) = hash;
	i_o.tt = LUA_TBOOLEAN;
	L->top++;
	luaA_pushobject(L, &i_o);

	return 1;
}
