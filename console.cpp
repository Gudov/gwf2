#include "console.h"
#include <Windows.h>
#include <fcntl.h>
#include <io.h>

Console* Console::getInstance()
{
	static Console* singleton = nullptr;
	if (!singleton) {
		singleton = new Console;
	}
	return singleton;
}

Console::Console() {
	redirectIOToConsole();
	enableStdout(true);
}

void Console::redirectIOToConsole() {
	int hConHandle;
	long lStdHandle;

	AllocConsole();

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fpin = _fdopen(hConHandle, "r");

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fpout = _fdopen(hConHandle, "w");
}

void Console::enableStdout(bool state) {
	const char* pipe = state ? "CONOUT$" : "NUL:";
	freopen(pipe, "w", stdout);
}