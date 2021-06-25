#include <imgui.h>
#include "dump.h"
#include <functional>
#include "resourceWindow.h"
#include "luaHooksL2.h"
#include <algorithm>

using std::string;

static std::vector<std::string> cache;
static std::vector<std::string> resourses;
static string resource;
static int itemCount;

void GUI::Resource::draw()
{
	bool p_open = true;
	ImGuiWindowFlags window_flags = 0;
	if (ImGui::Begin("Resource window", &p_open, window_flags)) {
		static int count = 1;
		ImGui::InputInt("count", &count);

		if (ImGui::Button("-100'000")) { count -= 100'000; }
		ImGui::SameLine();
		if (ImGui::Button("-10'000")) { count -= 10'000; }
		ImGui::SameLine();
		if (ImGui::Button("-1'000")) { count -= 1'000; }
		ImGui::SameLine();
		if (ImGui::Button("+1'000")) { count += 1'000; }
		ImGui::SameLine();
		if (ImGui::Button("+10'000")) { count += 10'000; }
		ImGui::SameLine();
		if (ImGui::Button("+100'000")) { count += 100'000; }

		if (ImGui::Button("update")) {
			executeLuaByHook("C:\\war3\\res\\listitems.lua");
		}

		ImGui::BeginChild("Names");
		for (string name : resourses) {
			if (ImGui::Button(name.c_str())) {
				resource = name;
				itemCount = count;
				executeLuaByHook("C:\\war3\\res\\giveresauto.lua");
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void GUI::Resource::clearResourcesCache() {
	cache.clear();
}

void GUI::Resource::pushResource(std::string name) {
	cache.push_back(name);
}

void GUI::Resource::updateResourcesFromCache() {
	resourses.clear();
	
	for (auto str : cache) {
		resourses.push_back(str);
	}
	std::sort(resourses.begin(), resourses.end());
}

string GUI::Resource::getItemName()
{
	return resource;
}

int GUI::Resource::getItemCount()
{
	return itemCount;
}
