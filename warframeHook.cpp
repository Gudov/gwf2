#include "warframeHook.h"
#include "luaImplAssist.h"
#include <stdio.h>

FILE* webLog = nullptr;

__int64 __fastcall warframe_web_request_hook(char* a1, __int64* a2, unsigned int a3) {
	if (webLog == nullptr) {
		webLog = fopen("C:\\war3\\webLog.txt", "w");
	}
	fflush(webLog);
	char* ch = a1;
	if (a1[31] == -1)
		ch = *(char**)a1;
	printf("\nperform web request: %s\n", ch);
	fprintf(webLog, "perform web request: %s\n", ch);
	fflush(stdout);
	fflush(webLog);
	return warframe_web_request_implementation(a1, a2, a3);
}

__int64 __fastcall warframe_web_request_hook_2(char* a1, __int64* a2, unsigned int a3) {
	if (webLog == nullptr) {
		webLog = fopen("C:\\war3\\webLog.txt", "w");
	}
	fflush(webLog);
	char* ch = a1;
	if (a1[31] == -1)
		ch = *(char**)a1;
	printf("\nperform web_2 request: %s\n", ch);
	fprintf(webLog, "perform web_2 request: %s\n", ch);
	fflush(stdout);
	fflush(webLog);
	return warframe_web_request_implementation_2(a1, a2, a3);
}
