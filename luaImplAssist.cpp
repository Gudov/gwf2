#pragma once

#include "luaImplAssist.h"

extern "C"
{
	void* lua_load_bytecode_original;
	void* lua_load_original;

	int (*lua_pcall_original)(lua_State* L, int nargs, int nresults, int errfunc);

	void* warframe_lua_thread_resume_original;
	void* warframe_web_parse_original;
	void* warframe_moveOrCopyStr_original;
}