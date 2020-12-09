#include "luaLoader.h"
#include <string>
#include "luaImplAssist.h"

extern "C" {
#include <lua/lauxlib.h>
#include <lua/lstate.h>
#include <lua/lfunc.h>
#include <lua/lwops.h>
#include <lua/llex.h>
    extern void luaU_print(const Proto* f, int full);
	extern int str_dump(lua_State* L);
    extern int writer(lua_State* L, const void* b, size_t size, void* B);
}

struct LoadS {
    uint8_t* s;
    size_t size;
    size_t remain;
};

static uint8_t* __fastcall warframeReader(__int64 a1, LoadS* a2, size_t* size)
{
    __int64 v3; // rax
    uint8_t *result; // rax

    v3 = a2->size;
    if (a2->remain == v3)
    {
        *size = 0i64;
        result = 0i64;
    }
    else
    {
        *size = v3;
        a2->remain = a2->size;
        result = a2->s;
    }
    return result;
}

static lua_State* getLoadState() {
    static lua_State* loadState = nullptr;
    if (loadState == nullptr) {
        loadState = luaL_newstate();
    }

    return loadState;
}

bool loadLuaScriptFromString(lua_State* L, char* str, size_t sz, const char *name) {
	lua_State* loadState = getLoadState();
	int loadError = luaL_loadbuffer(loadState, str, sz, name);
    
    if (loadError) {
        lua_pop(loadState, 1);
        return false;
    }

    luaL_Buffer b;
    luaL_buffinit(loadState, &b);
    if (lua_dump(loadState, writer, &b) != 0)
        luaL_error(loadState, "unable to dump given function");
    luaL_pushresult(&b);

	size_t dumpSize;
	const char *dump = lua_tolstring(loadState, -1, &dumpSize);

    LoadS loadS;
    loadS.s = (uint8_t*)dump;
    loadS.size = dumpSize;
    loadS.remain = 0;
    loadError = lua_load_implementation(L, (long long)warframeReader, &loadS, name);

    lua_pop(loadState, 2);

    return loadError == 0;
}
