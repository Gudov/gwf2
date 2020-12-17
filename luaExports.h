#pragma once

extern "C" {
#include <lua/lapi.h>
#include <lua/lauxlib.h>
}

void loadLuaExports(lua_State *L);