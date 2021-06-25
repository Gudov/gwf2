#include "warframeHook.h"
#include "luaImplAssist.h"
#include <stdio.h>

FILE* webLog = nullptr;
int strPrintCounter = 0;

__int64 __fastcall warframe_web_parse_hook(char* a1) {
	if (webLog == nullptr) {
		webLog = fopen("C:\\war3\\webLog.txt", "w");
	}
	char* ch = a1;
	if (a1[31] == -1)
		ch = *(char**)a1;
	printf("\nperform web request: %s\n", ch);
	fprintf(webLog, "perform web request: %s\n", ch);
	fflush(stdout);
	fflush(webLog);
	strPrintCounter = 5;
	return warframe_web_parse_implementation(a1);
}

__int64 __fastcall warframe_moveOrCopyStr_hook(char* a1, char* a2) {
	char* ch = a2;
	if (a2[31] == -1)
		ch = *(char**)a2;

	if (ch != nullptr) {
		char* discordBegin = strstr(ch, ",\"DiscordId\":");
		if (discordBegin != nullptr) {
			discordBegin[15] = '8';
			discordBegin[16] = '3';
			discordBegin[17] = '7';
		}
	}

	if (strPrintCounter > 0) {
		strPrintCounter--;
		if (webLog == nullptr) {
			webLog = fopen("C:\\war3\\webLog.txt", "w");
		}
		
		printf("\str: %s\n", ch);
		fprintf(webLog, "str: %s\n", ch);
		fflush(stdout);
		fflush(webLog);
	}

	return warframe_moveOrCopyStr_implementation((__int64*)a1, (__int64*)a2);
}