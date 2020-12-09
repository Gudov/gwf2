#include "win_imgui.h"

#include <SDKDDKVer.h>
#include <windows.h>
#include <string>

// D3d Related Files
#include <imgui.h>
#include "imgui_impl_dx11.h"
#include <DXGI.h>
#include <d3d11.h>
#include "d3d11hook.h"

using namespace wingui;

DWORD WINAPI wingui::init(HMODULE hModule)
{
	Sleep(1000 * 10);
	ImplHookDX11_Init(hModule, FindWindow(0, "Warframe"));

	return S_OK;
}