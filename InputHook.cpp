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
	if (GUI::isMouseEnabled() && !GUI::isInputFullCapture()) {
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			GetIO().MouseDown[0] = true;
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		case WM_LBUTTONUP:
			GetIO().MouseDown[0] = false;
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		case WM_RBUTTONDOWN:
			GetIO().MouseDown[1] = true;
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		case WM_RBUTTONUP:
			GetIO().MouseDown[1] = false;
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		case WM_MBUTTONDOWN:
			GetIO().MouseDown[2] = true;
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		case WM_MBUTTONUP:
			GetIO().MouseDown[2] = false;
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		case WM_MOUSEWHEEL:
			GetIO().MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		case WM_MOUSEMOVE:
			GetIO().MousePos.x = (signed short)(lParam); GetIO().MousePos.y = (signed short)(lParam >> 16);
			if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
			break;
		}

		return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
	}

	if (!GUI::isMouseEnabled()) {
		return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
	}

	switch (uMsg)
	{
	case WM_KEYDOWN:
		GetIO().AddInputCharacter(wParam);
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_KEYUP:
		break;
	case WM_LBUTTONDOWN:
		GetIO().MouseDown[0] = true; 
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_LBUTTONUP:
		GetIO().MouseDown[0] = false;
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_RBUTTONDOWN:
		GetIO().MouseDown[1] = true;
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_RBUTTONUP:
		GetIO().MouseDown[1] = false;
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_MBUTTONDOWN:
		GetIO().MouseDown[2] = true;
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_MBUTTONUP:
		GetIO().MouseDown[2] = false;
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_MOUSEWHEEL:
		GetIO().MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	case WM_MOUSEMOVE:
		GetIO().MousePos.x = (signed short)(lParam); GetIO().MousePos.y = (signed short)(lParam >> 16);
		if (GUI::isInputFullCapture()) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
		break;
	}

	return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}