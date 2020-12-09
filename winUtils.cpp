#include <Windows.h>
#include <tlhelp32.h>
#include "console.h"

void pauseAllThreads(bool paused) {
	FILE* consolePipe = Console::getInstance()->fpout;
	fprintf(consolePipe, "%s all threads\n", paused ? "Suspend" : "Resume");
	fflush(consolePipe);
	DWORD currentThreadId = GetCurrentThreadId();
	DWORD currentProcessId = GetCurrentProcessId();
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te)) {
			do {
				if (te.th32OwnerProcessID == currentProcessId && te.th32ThreadID != currentThreadId) {
					HANDLE th = OpenThread(THREAD_SUSPEND_RESUME, false, te.th32ThreadID);
					if (paused) {
						Wow64SuspendThread(th);
					}
					else {
						ResumeThread(th);
					}
				}
				te.dwSize = sizeof(te);
			} while (Thread32Next(h, &te));
		}
		CloseHandle(h);
	}
}