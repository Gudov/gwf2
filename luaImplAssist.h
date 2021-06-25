#pragma once
#include "Warframe.x64.exe.h"

extern "C"
{
	extern __int64 __fastcall lua_load_bytecode_implementation(script_mng *mng, script_data *data);
	extern __int64 __fastcall lua_load_implementation(lua_State* L, __int64 a2, void* data, const char* chunkname);
	extern __int64 __fastcall lua_resume_implementaion(lua_State* L, int nargs);
	extern __int64 __fastcall warframe_lua_thread_resume_implementation(script_mng*mng, __int64 *a2);
	extern __int64 __fastcall warframe_web_parse_implementation(char* a1);
	extern __int64 __fastcall warframe_moveOrCopyStr_implementation(__int64 *a1, __int64* a2);

	typedef __int64 (*lua_load_bytecode_type)(script_mng*, script_data*);
	typedef __int64 (*lua_load_type)(lua_State* L, void* reader, rawLuaData* data, const char* chunkname);
	typedef int (*lua_pcall_type)(lua_State* L, int nargs, int nresults, int errfunc);

	extern lua_load_bytecode_type lua_load_bytecode_original;
	extern lua_load_type lua_load_original;
	extern lua_pcall_type lua_pcall_original;

	typedef __int64 (*warframe_lua_thread_resume_type)(script_mng* mng, __int64* a2);
	typedef __int64 (*warframe_web_parse_type)(char *a1);
	typedef __int64 (*warframe_moveOrCopyStr_type)(char* a1, char* a2);
	typedef __int64 (*warframe_print1_type)(void* cont, char* str, size_t size);
	typedef __int64 (*warframe_print2_type)(void* cont, char* str);

	typedef __int64 (*warframe_resManager_5_type)(__int64 self, __int64 a2, void* name, void* a4);
	typedef __int64 (*warframe_resManager_8_type)(__int64 self, __int64 a2, void* name, void* a4);

	extern warframe_lua_thread_resume_type warframe_lua_thread_resume_original;
	extern warframe_web_parse_type warframe_web_parse_original;
	extern warframe_moveOrCopyStr_type warframe_moveOrCopyStr_original;
	extern warframe_print1_type warframe_print1;
	extern warframe_print2_type warframe_print2;

	extern warframe_resManager_5_type warframe_resManager_5_original;
	extern warframe_resManager_8_type warframe_resManager_8_original;
};

void loadWarframeLuaApi();