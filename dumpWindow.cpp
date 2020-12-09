#include "dumpWindow.h"
#include <imgui.h>
#include "dump.h"
#include <functional>

namespace GUI {
	namespace DrawWindow {
		
	}
}

using namespace GUI;

void DumpWindow::drawMenuBar() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("dump all", NULL)) {
				DumpManger::saveAll();
			}
			if (ImGui::MenuItem("clear", NULL)) {
				DumpManger::clear();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

static void drawFile(DumpedData* folder, std::string name) {
	if (ImGui::BeginMenu(name.c_str())) {
		ImGui::MenuItem("save");
		ImGui::EndMenu();
	}
}

static void drawFolder(DumpedData* folder, std::string name) {
	try {
		auto map = std::get<DumpedData::Map>(folder->data);
		if (ImGui::TreeNode(name.c_str())) {
			for (auto file : map) {
				if (file.second->isFolder) {
					drawFolder(file.second, file.first);
				}
				else {
					drawFile(file.second, file.first);
				}
			}
			ImGui::TreePop();
		}
	}
	catch (const std::bad_variant_access&) {
		ImGui::Text("folder load error");
	}
}

void DumpWindow::drawFiles() {
	DumpedData *root = DumpManger::getRoot();
	drawFolder(root, "/");
}

void DumpWindow::draw() {
	bool p_open = true;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize;
	if (ImGui::Begin("Dump window", &p_open, window_flags)) {
		drawMenuBar();
		drawFiles();
	}
}
