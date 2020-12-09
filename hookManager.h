#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

struct hookConfig {
	std::string pattern;
	std::string debugName;
	void* hook;
};

class HookManager {
public:
	void* setupHook(hookConfig hookData);

	std::unordered_map<std::string, bool> hooksStatus;

	void* findSignature(unsigned char* data, const char* combopattern);
	unsigned char* getWarframeExe();

//private:
	void* findSignature(unsigned char* data, unsigned char* sig, char* pattern);
	void parsePattern(char* combo, char* pattern, char* mask);
	void placeHook(void* target_func, void* hook_func);
};