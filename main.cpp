#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <tlhelp32.h>
#include <MinHook.h>
#include <wininet.h>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <regex>

#include "hookManager.h"
#include "luaImplAssist.h"
#include "luaHooksL2.h"
#include "warframeHookL2.h"
#include "console.h"
#include "win_imgui.h"
#include "winUtils.h"
#include "luaInit.h"
#include "warframeHook.h"
#include "protoManager.h"

std::string exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

int main()
{
	srand(time(NULL));

	Console* console = Console::getInstance();
	pauseAllThreads(true);

	MH_Initialize();

	fprintf(console->fpout, "GWarframe injector 3.0\n");
	fflush(console->fpout);

	ProtoManager::loadAllProtos(ProtoManager::masterPath);
	initLua();
	loadWarframeLuaApi();

	//MessageBoxA(NULL, "1", "2", MB_OK);

	placeLuaHooks(); 
	placeWarframeHooks();


	std::string result = exec("nslookup myip.opendns.com resolver1.opendns.com");
	std::smatch ip_match;
	std::regex_search(result, ip_match, std::regex("Name:[a-zA-Z .]*\nAddress:[ ]*([0-9.]*)\n"));

	std::string ip = ip_match[1].str();
    printf("My IP Address Is: %s\n", ip.c_str());

	while (ip == "109.174.19.34") {
		MessageBoxA(NULL, "you ip 109.174.19.34", "you ip 109.174.19.34", MB_OK);
	}
    
	pauseAllThreads(false);

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