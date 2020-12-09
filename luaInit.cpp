#include "luaInit.h"
#include <string>
#include "luaImplAssist.h"
#include "luaLoader.h"

extern "C" {
#include <lua/lstate.h>
#include <lua/lopcodes.h>
}

void initLua() {
	setOpcodeConversion(OP_TAILCALL, 0);
	setOpcodeConversion(OP_CLOSURE, 1);
	setOpcodeConversion(OP_EQ, 2);
	setOpcodeConversion(OP_DIV, 3);
	setOpcodeConversion(OP_CONCAT, 5);
	setOpcodeConversion(OP_GETTABLE, 6);
	setOpcodeConversion(OP_SETLIST, 7);
	setOpcodeConversion(OP_LOADK, 8);
	setOpcodeConversion(OP_SETGLOBAL, 9);
	setOpcodeConversion(OP_JMP, 0xA);
	setOpcodeConversion(OP_TFORLOOP, 0xB);
	setOpcodeConversion(OP_SETUPVAL, 0xC);
	setOpcodeConversion(OP_NOT, 0xD);
	setOpcodeConversion(OP_VARARG, 0xE);
	setOpcodeConversion(OP_GETUPVAL, 0xF);
	setOpcodeConversion(OP_ADD, 0x10);
	setOpcodeConversion(OP_RETURN, 0x11);
	setOpcodeConversion(OP_GETGLOBAL, 0x12);
	setOpcodeConversion(OP_LEN, 0x13);
	setOpcodeConversion(OP_MUL, 0x14);
	setOpcodeConversion(OP_NEWTABLE, 0x15);
	setOpcodeConversion(OP_TESTSET, 0x16);
	setOpcodeConversion(OP_SETTABLE, 0x17);
	setOpcodeConversion(OP_UNM, 0x18);
	setOpcodeConversion(OP_MOD, 0x19);
	setOpcodeConversion(OP_LT, 0x1A);
	setOpcodeConversion(OP_FORLOOP, 0x1B);
	setOpcodeConversion(OP_CALL, 0x1C);
	setOpcodeConversion(OP_LE, 0x1D);
	setOpcodeConversion(OP_LOADBOOL, 0x1E);
	setOpcodeConversion(OP_FORPREP, 0x1F);
	setOpcodeConversion(OP_TEST, 0x21);
	setOpcodeConversion(OP_POW, 0x22);
	setOpcodeConversion(OP_SELF, 0x23);
	setOpcodeConversion(OP_SUB, 0x24);
	setOpcodeConversion(OP_MOVE, 0x25);
	setOpcodeConversion(OP_CLOSE, 0x26);
	setOpcodeConversion(OP_LOADNIL, 0x27);
}

bool luaInitScriptExecuted = false;

void reloadLuaInitScript() {
	luaInitScriptExecuted = false;
}

void executeLuaInitScript(lua_State* L) {
	if (luaInitScriptExecuted) {
		return;
	}
	luaInitScriptExecuted = true;

	const std::string initScriptPath = "C:\\war3\\autorun.lua";

	char* buff;
	FILE* f = fopen(initScriptPath.c_str(), "r");
	fseek(f, 0L, SEEK_END);
	size_t sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	buff = (char*)malloc(sz + 2);
	memset(buff, 0, sz + 2);
	fread(buff, sz, 1, f);
	fclose(f);

	buff[sz] = 0;

	loadLuaScriptFromString(L, buff, strlen(buff), "autorun");
	lua_pcall_original(L, 0, 0, 0);
}
