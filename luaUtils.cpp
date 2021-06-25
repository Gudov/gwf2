#include "luaUtils.h"
#include "luaExports.h"
#include "luaLoader.h"
#include "luaImplAssist.h"
#include "luaLoader.h"
#include "luaExports.h"
#include "bytecodeParser.h"
#include "console.h"

EXTERN_C_START
int db_errorfb(lua_State* L);
EXTERN_C_END

static size_t getFileSize(const char* name) {
	HANDLE hFile = CreateFile(name, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1; // error condition, could call GetLastError to find out more

	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size))
	{
		CloseHandle(hFile);
		return -1; // error condition, could call GetLastError to find out more
	}

	CloseHandle(hFile);
	return size.QuadPart;
}

static size_t fucking_proper_strlen(const char* str) {
	size_t len = 0;
	do {
		len++;
	} while (str[len]);

	return len;
}

static void fucking_proper_fread(FILE *f, char *data, size_t sz) {
	while (sz >= 4000) {
		fread(data, 4000, 1, f);
		sz -= 4000;
		data += 4000;
	}

	if (sz > 0) {
		fread(data, sz, 1, f);
	}
}

bool loadAndExecuteLua(lua_State* L, std::string file, bool loadExports) {
	char* buff;
	FILE* f = fopen(file.c_str(), "r");
	//fseek(f, 0L, SEEK_END);
	//size_t sz = ftell(f);
	//fseek(f, 0, SEEK_SET);
	size_t sz = getFileSize(file.c_str());
	printf("load file %s with size %d ", file.c_str(), (int)sz);
	buff = (char*)malloc(sz + 11);
	memset(buff, 0, sz + 10);
	//fread(buff, sz, 1, f);
	fucking_proper_fread(f, buff, sz);
	fclose(f);

	//buff[sz] = 0;

	if (loadExports) {
		loadLuaExports(L);
	}

	printf("with len %d\n", (int)fucking_proper_strlen(buff));

	int result = loadLuaScriptFromString(L, buff, fucking_proper_strlen(buff), "autorun");
	if (result) {
		if (result = lua_pcall_original(L, 0, 0, 0) != 0) {
			fprintf(Console::getInstance()->fpout, "pcall error: %d\n", result);
			fprintf(Console::getInstance()->fpout, "txt: %s\n", lua_tostring(L, 0));
			db_errorfb(L);
			const char* traceback = lua_tostring(L, -1);
			fprintf(Console::getInstance()->fpout, "traceback: %s\n", traceback);
			fflush(Console::getInstance()->fpout);
			lua_pop(L, 1);
			return false;
		}
	}
	else {
		fprintf(Console::getInstance()->fpout, "error on lua load\n");
		fflush(Console::getInstance()->fpout);
		return false;
	}

	return true;
}