#pragma once

#include "luaImplAssist.h"
#include "hookUtils.h"

extern "C"
{
	lua_load_bytecode_type lua_load_bytecode_original;
	lua_load_type lua_load_original;
	lua_pcall_type lua_pcall_original;

	warframe_lua_thread_resume_type warframe_lua_thread_resume_original;
	warframe_web_parse_type warframe_web_parse_original;
	warframe_moveOrCopyStr_type warframe_moveOrCopyStr_original;
	warframe_print1_type warframe_print1;
	warframe_print2_type warframe_print2;

	warframe_resManager_5_type warframe_resManager_5_original;
	warframe_resManager_8_type warframe_resManager_8_original;
}

const char* lua_pcall_pattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 41 8B F8 8B F2 48 8B D9 45 85 C9 75 0C 4C 8B 59 40 45 33 D2 E9 ? ? ? ? 7E 23";

void loadWarframeLuaApi() {
	lua_pcall_original = findSignature<lua_pcall_type>(getWarframeExe(), lua_pcall_pattern);
}