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
			"4C 8B DC 49 89 5B 10 49 89 6B 18 49 89 73 20 57 48 81 EC ? ? ? ? 48 8B E9 49 89 4B F0 33 C9 49 89 53 E0 4D 85 C9 4D 89 43 E8 49 89 4B D0 4D 8D 43 A8 49 89 4B D8 48 8D 15 ? ? ? ? C7 84 24",
			"lua_load",
			lua_load_hook
		});

	warframe_web_parse_original = hookManager->setupHook({
			"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 50 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 48 8B F1 4C 8D 3D ? ? ? ? 45 33 F6 4B 8B 3C 3E 48 85 FF 74 7C",
			"warframe_web_parse",
			warframe_web_parse_hook
		});

	warframe_moveOrCopyStr_original = hookManager->setupHook({
			"48 3B D1 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 20 0F B6 42 1F 48 8B DA 48 8B F9 3C FF 75 6C 83 7A 0C FF 75 4D 38 41 1F 75 24 83 79 0C FF 74 1E 48 8B 09 E8 ? ? ? ? 0F B6 43 1F 3C FF 74 0E 0F BE C0",
			"warframe_moveOrCopyStr",
			warframe_moveOrCopyStr_hook
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
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)wingui::init, hinstDLL, NULL, NULL);
	}
	return TRUE;
}