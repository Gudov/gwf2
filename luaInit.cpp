#include "luaInit.h"
#include <string>
#include "luaImplAssist.h"
#include "luaLoader.h"
#include "luaExports.h"
#include "bytecodeParser.h"
#include "console.h"
#include "luaUtils.h"

extern "C" {
#include <lua/lstate.h>
#include <lua/lopcodes.h>
}

void initLua() {
	int opcodes[40];
	parseOpcodes(opcodes);

	for (int i = 0; i < 40; i++) {
		setOpcodeConversion(i, opcodes[i]);
	}
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

	loadAndExecuteLua(L, initScriptPath, true);

	/*char* buff;
	FILE* f = fopen(initScriptPath.c_str(), "r");
	fseek(f, 0L, SEEK_END);
	size_t sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	buff = (char*)malloc(sz + 2);
	memset(buff, 0, sz + 2);
	fread(buff, sz, 1, f);
	fclose(f);

	buff[sz] = 0;

	loadLuaExports(L);

	int result = loadLuaScriptFromString(L, buff, strlen(buff), "autorun");
	if (result) {
		if (result = lua_pcall_original(L, 0, 0, 0) != 0) {
			fprintf(Console::getInstance()->fpout, "pcall error: %d\n", result);
			fprintf(Console::getInstance()->fpout, "txt: %s\n", lua_tostring(L, 0));
			fflush(Console::getInstance()->fpout);
			lua_pop(L, 1);
		}
	} else {
		fprintf(Console::getInstance()->fpout, "error on lua load\n");
		fflush(Console::getInstance()->fpout);
	}*/
}
