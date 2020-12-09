#pragma once
#include <stdio.h>

class Console
{
public:
	static Console* getInstance();
	Console();

	void enableStdout(bool state);

	FILE* fpout;
	FILE* fpin;
private:
	void redirectIOToConsole();
};