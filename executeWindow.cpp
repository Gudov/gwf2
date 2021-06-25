#include "executeWindow.h"
#include <imgui.h>
#include <vector>
#include <string>
#include <filesystem>
#include <Windows.h>
#include "luaHooksL2.h"

using std::string;
using std::vector;
using namespace GUI;

static void read_directory(const string& name, const string& format, vector<string>& v)
{
	string pattern(name);
	pattern.append("\\*.");
	pattern.append(format);
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

void ExecuteWindow::draw() {
	bool p_open = true;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize;
	if (ImGui::Begin("Execute window", &p_open, window_flags)) {
		static bool init = false;
		static std::vector<string> files;
		if (ImGui::Button("*refresh*")) {
			init = false;
		}

		if (!init) {
			init = true;
			files.clear();
			read_directory("C:\\war3\\run", "lua", files);
		}

		for (string name : files) {
			if (ImGui::Button(name.c_str())) {
				executeLuaByHook("C:\\war3\\run\\" + name);
			}
		}
	}
	ImGui::End();
}