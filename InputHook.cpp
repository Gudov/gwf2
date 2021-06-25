#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>

// D3d Related Files
#include <imgui.h>
#include "imgui_impl_dx11.h"
#include <DXGI.h>
#include <d3d11.h>
//#include <menu.h>
#include "InputHook.h"
#include "d3d11hook.h"
#include "mainMenu.h"

using namespace ImGui;

WNDPROC	oWndProc;

void InputHook::Init(HWND hWindow)
{
	oWndProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
}

void InputHook::Remove(HWND hWindow)
{
	SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)oWndProc);
}

LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool keyboardOverride = GUI::isInputFullCapture();
	ImGuiIO& io = ImGui::GetIO();
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		GetIO().MouseDown[0] = true;
		break;
	case WM_LBUTTONUP:
		GetIO().MouseDown[0] = false;
		break;
	case WM_RBUTTONDOWN:
		GetIO().MouseDown[1] = true;
		break;
	case WM_RBUTTONUP:
		GetIO().MouseDown[1] = false;
		break;
	case WM_MBUTTONDOWN:
		GetIO().MouseDown[2] = true;
		break;
	case WM_MBUTTONUP:
		GetIO().MouseDown[2] = false;
		break;
	case WM_MOUSEWHEEL:
		GetIO().MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		break;
	case WM_MOUSEMOVE:
		GetIO().MousePos.x = (signed short)(lParam); GetIO().MousePos.y = (signed short)(lParam >> 16);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		if (keyboardOverride) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		if (keyboardOverride) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		if (keyboardOverride) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	}

	return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}