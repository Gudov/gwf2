#pragma once

#include <string>

extern "C" {
#include <lua/lstate.h>
#include <lua/lopcodes.h>
}


bool loadAndExecuteLua(lua_State* L, std::string file, bool loadExports);