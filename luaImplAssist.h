#pragma once
#include "Warframe.x64.exe.h"

extern "C"
{
	extern __int64 __fastcall lua_load_bytecode_implementation(script_mng *mng, script_data *data);
	extern __int64 __fastcall lua_load_implementation(lua_State* L, __int64 a2, void* data, const char* chunkname);
	extern __int64 __fastcall lua_resume_implementaion(lua_State* L, int nargs);
	extern __int64 __fastcall warframe_lua_thread_resume_implementation(script_mng*mng, __int64 *a2);
	extern __int64 __fastcall warframe_web_request_implementation(char* a1, __int64* a2, unsigned int a3);
	extern __int64 __fastcall warframe_web_request_implementation_2(char* a1, __int64* a2, unsigned int a3);

	extern void* lua_load_bytecode_original;
	extern void* lua_load_original;
	extern int (*lua_pcall_original)(lua_State* L, int nargs, int nresults, int errfunc);

	extern void* warframe_web_request_original;
	extern void* warframe_web_request_original_2;
	extern void* warframe_lua_thread_resume_original;
};