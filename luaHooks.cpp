#include "luaHooks.h"
#include "luaImplAssist.h"
#include "console.h"
#include "dump.h"
#include "luaInit.h"

EXTERN_C_START
#include <lua/lwops.h>
#include <lua/lapi.h>
#include <lua/lauxlib.h>
EXTERN_C_END

#include <chrono>
#include <string>

bool callInsideLuaLoadBytecode = false;
std::string luaFilePath;
std::string luaFileName;

__int64 __fastcall lua_load_bytecode_hook(script_mng* mng, script_data* data) {
	std::string fullPath = data->path;
	fullPath += data->name->_str;

	luaFilePath = data->path;
	luaFileName = data->name->_str;

	fprintf(Console::getInstance()->fpout, "Loading lua: %s\n", fullPath.c_str());
	fflush(Console::getInstance()->fpout);

	callInsideLuaLoadBytecode = true;
	__int64 result = lua_load_bytecode_implementation(mng, data);
	callInsideLuaLoadBytecode = false;
	return result;
}

__int64 __fastcall lua_load_hook(lua_State* L, void *reader, rawLuaData* data, const char* chunkname) {
	executeLuaInitScript(L);

	if (callInsideLuaLoadBytecode) {
		DumpManger::addData(luaFilePath, luaFileName, data->data, data->size); 
	}

	lua_getglobal(L, "hookLuaPostLoading");

	__int64 status = lua_load_implementation(L, (long long)reader, data, chunkname);

	if (callInsideLuaLoadBytecode) {
		auto closure = ((char*)L->top->value.p);
		Proto* proto = *((Proto**)(closure + 20));
	}

	lua_pushstring(L, luaFilePath.c_str());
	lua_pushstring(L, luaFileName.c_str());
	lua_pcall_original(L, 3, 1, 0);

	return status;
}


int warframeTickFps = 0;
int getWarframeTickFps() { return warframeTickFps; }
std::string tickMsg;
std::string getTickMsg() {
	return tickMsg;
}

__int64 __fastcall warframe_lua_thread_resume_hook(script_mng *mng, __int64 *a2) {
	lua_State* L = mng->L;
	static int warframeTickFpsBuffer = 0;
	warframeTickFpsBuffer++;

	static std::chrono::time_point lastFpsMesure = std::chrono::high_resolution_clock::now();
	static std::chrono::time_point lastTick = std::chrono::high_resolution_clock::now();
	std::chrono::time_point now = std::chrono::high_resolution_clock::now();
	long long fpsUpdateMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFpsMesure).count();
	long long tickDelayMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count();

	if (fpsUpdateMs > 1000) {
		warframeTickFps = warframeTickFpsBuffer;
		warframeTickFpsBuffer = 0;
		lastFpsMesure = now;
	}
	
	if (tickDelayMs > (1000.f/60.f)) {
		lastTick = now;
		lua_getglobal(L, "customTick");
		lua_pcall_original(L, 0, 1, 0);
		tickMsg = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	executeLuaInitScript(L);

	return warframe_lua_thread_resume_implementation(mng, a2);
}