#include "protoList.h"

#include "ImGui/imgui.h"
#include "protoManager.h"
#include "protoEditor.h"

namespace GUI::ProtoList {
	void draw() {
		bool p_open = true;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize;
		if (ImGui::Begin("Proto list window", &p_open, window_flags)) {
			static bool onlyLoaded = false;
			static bool onlyEnabled = false;
			ImGui::Checkbox("loaded", &onlyLoaded);
			ImGui::SameLine();
			ImGui::Checkbox("enabled", &onlyEnabled);
			ImGui::Separator();

			auto mastersMap = ProtoManager::getAllProtoMasters();
			for (auto &masters : (*mastersMap)) {
				if (ImGui::TreeNode(masters.first.c_str())) {
					for (auto it = masters.second.begin(); it != masters.second.end();) {
						auto &master = *it;
						bool remove = false;
						ImGui::PushStyleColor(ImGuiCol_Text, master->enabled ? ImVec4{127,255,127,255} : ImVec4{255,127,127,255});
						if (ImGui::BeginMenu(master->name.c_str())) {
							if (ImGui::MenuItem(master->enabled ? "disable" : "enable")) {
								master->enabled = !master->enabled;
							}

							if (ImGui::MenuItem("delete")) {
								remove = true;
							}
							if (ImGui::MenuItem("open editor")) {
								Proto* p = ProtoManager::getProto(master->filePath, master->fileName);
								if (p) {
									new ProtoEditor(master->filePath, master->fileName, p, master);
								}
							}

							ImGui::EndMenu();
						}

						if (remove) {
							it = masters.second.erase(it);
						} else {
							it++;
						}
					}
					ImGui::TreePop();
				}

			}
		}
		ImGui::End();
	}
}