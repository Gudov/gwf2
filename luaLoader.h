#pragma once

extern "C" {
#include <lua/lapi.h>
}

bool loadLuaScriptFromString(lua_State *L, char *str, size_t sz, const char* name);