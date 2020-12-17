#include "hookManager.h"
#include <Windows.h>
#include "console.h"

unsigned char* HookManager::getWarframeExe() {
	return (unsigned char*)GetModuleHandle("Warframe.x64.exe");
}

void* HookManager::setupHook(hookConfig hookData) {
	Console* console = Console::getInstance();
	void* funcPtr = findSignature(getWarframeExe(), hookData.pattern.c_str());
	hooksStatus[hookData.pattern] = (funcPtr != nullptr);
	if (funcPtr) {
		placeHook(funcPtr, hookData.hook);
	}

	fprintf(console->fpout, "Setup hook: %s %s\n", hookData.debugName.c_str(), funcPtr ? "Success" : "Fail");
	fflush(console->fpout);

	return funcPtr;
}

void HookManager::placeHook(void* target_func, void* hook_func) {
	const unsigned char hook_asm[0xC] = { 0x48, 0xB8, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
									0xFF, 0xE0 };
	char custom_hook_asm[0x14];
	memcpy(custom_hook_asm, hook_asm, 0xC);

	SIZE_T n;
	*(unsigned __int64*)(custom_hook_asm + 2) = (unsigned __int64)(hook_func);
	WriteProcessMemory(GetCurrentProcess(), LPVOID(target_func), custom_hook_asm, 0xC, &n);
}

void* HookManager::findSignature(unsigned char* data, const char* combopattern)
{
	unsigned char pattern[0x100] = {0x0};
	char mask[0x100] = {0x0};
	parsePattern((char*)combopattern, (char*)pattern, mask);
	return findSignature(data, pattern, mask);
}

void* HookManager::findSignature(unsigned char* data, unsigned char* sig, char* pattern)
{
	while (true) {
		data++;
		size_t offset = 0;
		while (data[offset] == sig[offset] || pattern[offset] == '?') {
			offset++;
			if (pattern[offset] == 0) {
				return data;
			}
		}
	}
}

void HookManager::parsePattern(char* combo, char* pattern, char* mask) {
	char lastChar = ' ';
	unsigned int j = 0;
	const size_t len = strlen(combo);

	for (unsigned int i = 0; i < len; i++) {
		if ((combo[i] == '?' || combo[i] == '*') && (lastChar != '?' && lastChar != '*')) {
			pattern[j] = mask[j] = '?';
			j++;
		}
		else if (isspace(lastChar)) {
			pattern[j] = lastChar = (char)strtol(&combo[i], 0, 16);
			mask[j] = 'x';
			j++;
		}
		lastChar = combo[i];
	}
	pattern[j] = mask[j] = '\0';
}
