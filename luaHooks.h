#pragma once
#include <string>
#include "Warframe.x64.exe.h"

__int64 __fastcall lua_load_bytecode_hook(script_mng* mng, script_data* data);
__int64 __fastcall lua_load_hook(lua_State* L, void* reader, rawLuaData* data, const char* chunkname);
__int64 __fastcall warframe_lua_thread_resume_hook(script_mng *mng, __int64 *a2);

int getWarframeTickFps();
std::string getTickMsg();