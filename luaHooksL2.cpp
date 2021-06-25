#include "luaHooks.h"
#include "luaImplAssist.h"
#include "console.h"
#include "dump.h"
#include "luaInit.h"
#include "protoManager.h"
#include "luaUtils.h"

EXTERN_C_START
#include <lua/lwops.h>
#include <lua/lapi.h>
#include <lua/lauxlib.h>
EXTERN_C_END

#include <chrono>
#include <string>
#include "luaExports.h"

#include "hookUtils.h"

const char* lua_load_bytecode_pattern = "4C 8B DC 55 53 56 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 72 50 49 89 7B 18 4D 89 6B D8 4C 8B EA";
//const char* lua_load_pattern = "4C 8B DC 49 89 5B 10 49 89 6B 18 56 57 41 56 48 81 EC ? ? ? ? 4C 8B F1 49 89 53 D0 49 89 4B E0 49 8D 43 C0 33 C9 4D 89 43 D8 4D 85 C9";
const char* lua_load_pattern = "E8 ? ? ? ? 49 8B 55 20 49 8B 4D 10 45 8B 45 1C 48 8B 49 10 48 89 4C 24 ? 48 8D 4C 24 ? 48 89 54 24";
const char* warframe_lua_thread_resume_pattern = "4C 8B DC 55 53 57 49 8D 6B A1 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 4D 89 73 E0 4C 8B F2";

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
	__int64 result = FnCast("lua_load_bytecode", lua_load_bytecode_original)(mng, data);
	callInsideLuaLoadBytecode = false;

	return result;
}

__int64 __fastcall lua_load_hook(lua_State* L, void* reader, rawLuaData* data, const char* chunkname) {
	executeLuaInitScript(L);

	lua_getglobal(L, "hookLuaPostLoading");

	__int64 status = FnCast("lua_load", lua_load_original)(L, reader, data, chunkname);

	if (callInsideLuaLoadBytecode) {
		auto closure = *((char**)(L->top - 1));
		Proto* proto = *((Proto**)(closure + 0x20));
		
		fixConst(L, proto);

		DumpManger::addData(luaFilePath, luaFileName, data->data, data->size, proto);
		ProtoManager::proccedProto(luaFilePath, luaFileName, proto);
	}

	lua_pushstring(L, luaFilePath.c_str());
	lua_pushstring(L, luaFileName.c_str());
	lua_pcall_original(L, 3, 1, 0);

	return status;
}

bool executeLua = false;
std::string luaToExeute = "";
int warframeTickFps = 0;
int getWarframeTickFps() { return warframeTickFps; }
std::string tickMsg;
std::string getTickMsg() {
	return tickMsg;
}

void executeLuaByHook(std::string fileName) {
	luaToExeute = fileName;
	executeLua = true;
}

__int64 __fastcall warframe_lua_thread_resume_hook(script_mng* mng, __int64* a2) {
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

	if (tickDelayMs > (1000.f / 60.f)) {
		lastTick = now;
		lua_getglobal(L, "customTick");
		lua_pcall_original(L, 0, 1, 0);
		tickMsg = lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	executeLuaInitScript(L);

	if (executeLua) {
		executeLua = false;
		loadAndExecuteLua(L, luaToExeute, false);
	}

	return FnCast("warframe_lua_thread_resume", warframe_lua_thread_resume_original)(mng, a2);
}

void placeLuaHooks() {
	lua_load_bytecode_original = findSignature<lua_load_bytecode_type>(getWarframeExe(), lua_load_bytecode_pattern);
	placeHook("lua_load_bytecode", lua_load_bytecode_original, lua_load_bytecode_hook);

	lua_load_original = findSignature<lua_load_type>(getWarframeExe(), lua_load_pattern);
	int offset = *(int*)((uint64_t)lua_load_original + 1);
	lua_load_original = lua_load_type((uint64_t)lua_load_original + 5 + offset);
	fprintf(Console::getInstance()->fpout, "lua_load final %X\n", lua_load_original);
	fflush(Console::getInstance()->fpout);
	placeHook("lua_load", lua_load_original, lua_load_hook);

	warframe_lua_thread_resume_original = findSignature<warframe_lua_thread_resume_type>(getWarframeExe(), warframe_lua_thread_resume_pattern);
	placeHook("warframe_lua_thread_resume", warframe_lua_thread_resume_original, warframe_lua_thread_resume_hook);
}