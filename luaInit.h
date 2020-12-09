#pragma once

extern "C" {
#include <lua/lwops.h>
#include <lua/lapi.h>
#include <lua/lauxlib.h>
}

void initLua();
void executeLuaInitScript(lua_State *L);
void reloadLuaInitScript();