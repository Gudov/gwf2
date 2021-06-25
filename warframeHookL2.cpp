#include "warframeHook.h"
#include "luaImplAssist.h"
#include <stdio.h>

#include "hookUtils.h"

FILE* webLog = nullptr;
int strPrintCounter = 0;

const char* warframe_web_parse_pattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 50 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 48 8B F1 4C 8D 3D ? ? ? ? 45 33 F6 4B 8B 3C 3E 48 85 FF 74 7C";
const char* warframe_moveOrCopyStr_pattern = "48 3B D1 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 20 0F B6 42 1F 48 8B DA 48 8B F9 3C FF 75 6C 83 7A 0C FF 75 4D 38 41 1F 75 24 83 79 0C FF 74 1E 48 8B 09 E8 ? ? ? ? 0F B6 43 1F 3C FF 74 0E 0F BE C0";
const char* warframe_print1_pattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 59 28 49 8B F0 49 3B D8 48 8B EA 48 8B F9 49 0F 46 D8 49 2B D8 83 79 20 00";
const char* warframe_print2_pattern = "40 53 48 83 EC 60 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 48 8B C2 48 8B D9 48 8B C8 BA ? ? ? ? E8 ? ? ? ?";

const char* warframe_resmanager_5_pattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B F9 49 8B F1 48 8B CA 49 8B E8 48 8B DA E8 ? ? ? ? 48 8D 0D";
const char* warframe_resmanager_8_pattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B F9 49 8B F1 48 8D 0D ? ? ? ? 49 8B E8 48 8B DA E8";

const char* warframe_discord_sdk_pattern = "2F 54 6F 6F 6C 73 2F 57 69 6E 64 6F 77 73 2F 78 36 34 2F 64 69 73 63 6F 72 64 5F 67 61 6D 65 5F 73 64 6B 2E 64 6C 6C 00";
const char* warframe_steam_api_pattern = "2F 54 6F 6F 6C 73 2F 57 69 6E 64 6F 77 73 2F 78 36 34 2F 73 74 65 61 6D 5F 61 70 69 36 34 2E 64 6C 6C 00";

__int64 __fastcall warframe_web_parse_hook(char* a1) {
	if (webLog == nullptr) {
		webLog = fopen("C:\\war3\\webLog.txt", "w");
	}
	char* ch = a1;
	if (a1[31] == -1)
		ch = *(char**)a1;
	//printf("\nperform web request: %s\n", ch);
	fprintf(webLog, "perform web request: %s\n", ch);
	fflush(stdout);
	fflush(webLog);
	strPrintCounter = 5;

	return FnCast("warframe_web_parse", warframe_web_parse_original)(a1);
}

__int64 __fastcall warframe_moveOrCopyStr_hook(char* a1, char* a2) {
	char* ch = a2;
	if (a2[31] == -1)
		ch = *(char**)a2;

	if (ch != nullptr) {
		char* discordBegin = strstr(ch, ",\"DiscordId\":");
		if (discordBegin != nullptr) {
			discordBegin[15] = '9';
			discordBegin[16] = '1';
			discordBegin[17] = '6';
		}

		char* steamId = strstr(ch, ",\"SteamId\":");
		if (steamId != nullptr) {
			steamId[15] = '9';
			steamId[16] = '1';
			steamId[17] = '6';
		}
	}

	if (strPrintCounter > 0) {
		strPrintCounter--;
		if (webLog == nullptr) {
			webLog = fopen("C:\\war3\\webLog.txt", "w");
		}

		//printf("\str: %s\n", ch);
		fprintf(webLog, "str: %s\n", ch);
		fflush(stdout);
		fflush(webLog);
	}

	return FnCast("warframe_moveOrCopyStr", warframe_moveOrCopyStr_original)(a1, a2);
}

__int64 __fastcall warframe_print1_hook(void* cont, char* str, size_t size) {
	fwrite(str, 1, size, stdout);
	return FnCast("warframe_print1", warframe_print1)(cont, str, size);
}

__int64 __fastcall warframe_print2_hook(void* cont, char* str) {
	fputs(str, stdout);
	return FnCast("warframe_print2", warframe_print2)(cont, str);
}

__int64 __fastcall warframe_resManager_5_hook(__int64 self, __int64 a2, void* name, void* a4) {
	char* str = (char*)name;
	if (*(((char*)name) + 31) == -1) {
		str = *(char**)name;
	}
	printf("resource: %s\n", str);
	return FnCast("warframe_resManager_5", warframe_resManager_5_original)(self, a2, name, a4);
}

__int64 __fastcall warframe_resManager_8_hook(__int64 self, __int64 a2, void* name, void* a4) {
	char* str = (char*)name;
	if (*(((char*)name) + 31) == -1) {
		str = *(char**)name;
	}
	printf("resource: %s\n", str);
	return FnCast("warframe_resManager_8", warframe_resManager_8_original)(self, a2, name, a4);
}

void breakConstStr(const char *str_pattern) {
	char* str = (char*)findSignatureImpl(getWarframeExe(), str_pattern);
	const unsigned char fake_str[0xC] = { 0x54 };

	DWORD oldFlags, newFlags;
	VirtualProtect(str, 0x10, PAGE_EXECUTE_WRITECOPY, &oldFlags);

	SIZE_T n;
	WriteProcessMemory(GetCurrentProcess(), LPVOID(str + 2), fake_str, 0x1, &n);
	printf("str writen bytes %d\ into %08llX %s\n", (int)n, str, str);

	VirtualProtect(str, 0x10, oldFlags, &newFlags);
}

void placeWarframeHooks() {
	warframe_web_parse_original = findSignature<warframe_web_parse_type>(getWarframeExe(), warframe_web_parse_pattern);
	placeHook("warframe_web_parse", warframe_web_parse_original, warframe_web_parse_hook);

	warframe_moveOrCopyStr_original = findSignature<warframe_moveOrCopyStr_type>(getWarframeExe(), warframe_moveOrCopyStr_pattern);
	placeHook("warframe_moveOrCopyStr", warframe_moveOrCopyStr_original, warframe_moveOrCopyStr_hook);

	warframe_print1 = findSignature<warframe_print1_type>(getWarframeExe(), warframe_print1_pattern);
	//placeHook("warframe_print1", warframe_print1, warframe_print1_hook);

	warframe_print2 = findSignature<warframe_print2_type>(getWarframeExe(), warframe_print2_pattern);
	//placeHook("warframe_print2", warframe_print2, warframe_print2_hook);

	warframe_resManager_5_original = findSignature<warframe_resManager_5_type>(getWarframeExe(), warframe_resmanager_5_pattern);
	//placeHook("warframe_resManager_5", warframe_resManager_5_original, warframe_resManager_5_hook);

	warframe_resManager_8_original = findSignature<warframe_resManager_8_type>(getWarframeExe(), warframe_resmanager_8_pattern);
	//placeHook("warframe_resManager_8", warframe_resManager_8_original, warframe_resManager_8_hook);

	breakConstStr(warframe_discord_sdk_pattern);
	breakConstStr(warframe_steam_api_pattern);
}