#include "luaHooks.h"
#include "luaImplAssist.h"
#include "console.h"
#include "dump.h"
#include "luaInit.h"
#include "protoManager.h"

EXTERN_C_START
#include <lua/lwops.h>
#include <lua/lapi.h>
#include <lua/lauxlib.h>
EXTERN_C_END

#include <chrono>
#include <string>
#include "luaExports.h"

bool callInsideLuaLoadBytecode = false;
std::string luaFilePath;
std::string luaFileName;

/*
void placeHooksDeprecated(HookManager* hookManager) {
	lua_load_bytecode_original = hookManager->setupHook({
			"4C 8B DC 55 53 56 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 72 50 49 89 7B 18 4D 89 6B D8 4C 8B EA",
			"lua_load_bytecode",
			lua_load_bytecode_hook
		});

	lua_load_original = hookManager->setupHook({
			"4C 8B DC 49 89 5B 10 49 89 6B 18 56 57 41 56 48 81 EC ? ? ? ? 4C 8B F1 49 89 4B E0 33 C9 49 89 53 D0 4D 85 C9 4D 89 43 D8 49 8D 43 C0 49 89 4B C0 49 89 4B C8 4D 8D 43 98 C7 84 24",
			"lua_load",
			lua_load_hook
		});

	warframe_web_parse_original = hookManager->setupHook({
			"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 50 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 48 8B F1 4C 8D 3D ? ? ? ? 45 33 F6 4B 8B 3C 3E 48 85 FF 74 7C",
			"warframe_web_parse",
			warframe_web_parse_hook
		});

	warframe_moveOrCopyStr_original = hookManager->setupHook({
			"48 3B D1 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 20 0F B6 42 1F 48 8B DA 48 8B F9 3C FF 75 6C 83 7A 0C FF 75 4D 38 41 1F 75 24 83 79 0C FF 74 1E 48 8B 09 E8 ? ? ? ? 0F B6 43 1F 3C FF 74 0E 0F BE C0",
			"warframe_moveOrCopyStr",
			warframe_moveOrCopyStr_hook
		});

	warframe_lua_thread_resume_original = hookManager->setupHook({
			"4C 8B DC 55 53 57 49 8D 6B A1 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 4D 89 73 E0 4C 8B F2",
			"warframe_lua_thread_resume",
			warframe_lua_thread_resume_hook
		});

	lua_pcall_original = (int(*)(lua_State*,int,int,int))hookManager->findSignature(
			hookManager->getWarframeExe(),
			"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 41 8B F8 8B F2 48 8B D9 45 85 C9 75 0C 4C 8B 59 40 45 33 D2 E9 ? ? ? ? 7E 23"
		);
}
*/


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

	lua_getglobal(L, "hookLuaPostLoading");

	__int64 status = lua_load_implementation(L, (long long)reader, data, chunkname);

	if (callInsideLuaLoadBytecode) {
		auto closure = *((char**)(L->top-1));
		Proto* proto = *((Proto**)(closure + 0x20));
		{
			printf("Proto: 0x%08X", proto);
		}
		
		DumpManger::addData(luaFilePath, luaFileName, data->data, data->size, proto);
		ProtoManager::proccedProto(luaFilePath, luaFileName, proto);
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