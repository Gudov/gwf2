#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <tlhelp32.h>

#include "hookManager.h"
#include "luaImplAssist.h"
#include "luaHooks.h"
#include "console.h"
#include "win_imgui.h"
#include "winUtils.h"
#include "luaInit.h"
#include "warframeHook.h"

void placeHooks(HookManager* hookManager) {
	pauseAllThreads(true);

	lua_load_bytecode_original = hookManager->setupHook({
			"4C 8B DC 55 53 56 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 72 50 49 89 7B 18 4D 89 6B D8 4C 8B EA",
			"lua_load_bytecode",
			lua_load_bytecode_hook
		});

	lua_load_original = hookManager->setupHook({
			"4C 8B DC 49 89 5B 10 49 89 6B 18 56 57 41 56 48 81 EC ? ? ? ? 4C 8B F1 49 89 4B E0 33 C9 49 89 53 D0 4D 85 C9 4D 89 43 D8 49 8D 43 C0 49 89 4B C0",
			"lua_load",
			lua_load_hook
		});

	warframe_lua_thread_resume_original = hookManager->setupHook({
			"4C 8B DC 55 53 57 49 8D 6B A1 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 4D 89 73 E0 4C 8B F2",
			"warframe_lua_thread_resume",
			warframe_lua_thread_resume_hook
		});

	lua_pcall_original = (int(*)(lua_State*,int,int,int))hookManager->findSignature(
			hookManager->getWarframeExe(),
			"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 41 8B F8 8B F2 48 8B D9 45 85 C9 75 0C 4C 8B 59 40 45 33 D2 E9 ? ? ? ? 7E 23"
		);

	warframe_web_request_original = hookManager->findSignature(hookManager->getWarframeExe(), "48 89 5C 24 ? 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 45 8B F8 4C 8B F2 48 8B F9 E8 ? ? ? ? 41 8B CF E8 ? ? ? ?");
	hookManager->placeHook(warframe_web_request_original, warframe_web_request_hook);
	warframe_web_request_original_2 = hookManager->findSignature((unsigned char *)warframe_web_request_original + 0x50, "48 89 5C 24 ? 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 45 8B F8 4C 8B F2 48 8B F9 E8 ? ? ? ? 41 8B CF E8 ? ? ? ?");
	hookManager->placeHook(warframe_web_request_original_2, warframe_web_request_hook_2);
	 
	//char test[256];
	//sprintf(test, "%x %x", first, warframe_web_request_original);
	//MessageBoxA(nullptr, test, "test init", MB_OK);
	/*warframe_web_request_original = hookManager->setupHook({
			"48 89 5C 24 ? 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 45 8B F8 4C 8B F2 48 8B F9 E8 ? ? ? ? 41 8B CF E8 ? ? ? ?",
			"warframe_web_request",
			warframe_web_request_hook
		});*/

	pauseAllThreads(false);
}

int main()
{
	srand(time(NULL));

	Console* console = Console::getInstance();
	fprintf(console->fpout, "GWarframe injector 3.0\n");

	initLua();

	HookManager *hookManager = new HookManager;
	placeHooks(hookManager);

	return 0;
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL,
	DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);
		//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)wingui::init, hinstDLL, NULL, NULL);
	}
	return TRUE;
}