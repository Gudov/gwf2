#include "protoEditor.h"
#include "ImGui/imgui.h"
#include "console.h"
#include <Windows.h>
#include <cstdio>
#include <memory>

using std::vector;

extern "C" {
	static const char* lua_getStr(TValue* val) {
		return svalue(val);
	}
}

namespace GUI {

	std::list<ProtoEditor*> editors;

	void ProtoEditor::drawAll() {
		ProtoEditor* toClose = nullptr;
		for (auto win : editors) {
			win->draw();
			if (win->isClosing()) {
				toClose = win;
			}
		}

		if (toClose) {
			editors.remove(toClose);
			delete toClose;
		}
	}

	ProtoEditor::ProtoEditor(std::string path, std::string name, Proto* p) : 
		path(path), name(name), p(p)
	{
		editors.push_back(this);
		master = new ProtoMaster();
		master->fileName = name;
		master->filePath = path;
		master->enabled = false;

		strcpy(masterName, master->name.c_str());
		ProtoManager::addProtoMaster(path, name, master);
	}

	ProtoEditor::ProtoEditor(std::string path, std::string name, Proto* p, ProtoMaster *master) :
		path(path), name(name), p(p), master(master)
	{
		strcpy(masterName, master->name.c_str());
		editors.push_back(this);
	}

	static void toClipboard(HWND hwnd, const std::string& s) {
		OpenClipboard(hwnd);
		EmptyClipboard();
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
		if (!hg) {
			CloseClipboard();
			return;
		}
		memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);
	}

	void ProtoEditor::draw() {
		bool p_open = true;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize;
		std::string winName = master->name;
		winName += " ";
		winName += path;
		winName += "/";
		winName += name; 
		if (ImGui::Begin(winName.c_str(), &p_open, window_flags)) {
			ImGui::Text("%s/%s", path.c_str(), name.c_str());
			ImGui::Text("%s", master->name.c_str());

			ImGui::InputText("name", masterName, 255);
			ImGui::SameLine();
			if (ImGui::Button("apply")) {
				ProtoManager::deleteProtoMaster(ProtoManager::masterPath + "\\" + master->name);
				master->name = masterName;
				ProtoManager::saveProtoMaster(master, ProtoManager::masterPath + "\\" + master->name);
			}

			ImGui::SameLine();
			if (ImGui::Button("close")) {
				this->close = true;
			}

			ImGui::Checkbox("enabled", &(master->enabled));
			ImGui::SameLine();
			if (ImGui::Button("apply")) {
				Proto* p = ProtoManager::getProto(master->filePath, master->fileName);
				if (p) {
					ProtoManager::applyProtoMaster(p, master);
				}
			}

			if (ImGui::Button("print")) {
				int kPath[32];
				std::string str;
				printConsts(str, p, 0, kPath);
				fprintf(Console::getInstance()->fpout, "%s", str.c_str());
			}

			ImGui::SameLine();

			if (ImGui::Button("copy")) {
				int kPath[32];
				std::string str;
				printConsts(str, p, 0, kPath);
				toClipboard(GetActiveWindow(), str);
			}

			ImGui::Separator();

			if (ImGui::TreeNode("changes")) {
				drawChanges();
				ImGui::TreePop();
			}
			char buff[64];
			sprintf(buff, "0x%08X", p);
			ImGui::Text(buff);
			if (ImGui::TreeNode("vars")) {
				vector<int> path;
				drawVars(p, path);
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void ProtoEditor::drawChanges() {
		for (int i = 0; i < master->fakeVars.size(); i++) {
			auto& var = master->fakeVars[i];
			std::string pathStr;
			for (auto &s : var.path) {
				pathStr += s;
				pathStr += '.';
			}
			pathStr.pop_back();
			pathStr.push_back(')');

			ImGui::Text(pathStr.c_str());
			ImGui::SameLine();
			if (var.val.tt == LUA_TNUMBER) {
				ImGui::InputDouble("", (double*)&(var.val.value.n));
			} else if (var.val.tt == LUA_TSTRING) {
				ImGui::InputText("", (char*)(var.val.value.gc), 511);
			}
			ImGui::SameLine();
			if (ImGui::Button("X")) {
				master->fakeVars[i] = master->fakeVars[master->fakeVars.size() - 1];
				master->fakeVars.pop_back();
			}
		}
	}

	template<typename ... Args>
	std::string stringFormat(const std::string& format, Args ... args)
	{
		int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size <= 0) { return "formatting error"; }
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	static void printPath(std::string &str, int level, int* path)
	{
		str += "  ";
		for (int i = 0; i <= level; i++) {
			str += stringFormat("%d.", path[i]);
		}
	}

	void ProtoEditor::printConsts(std::string &str, Proto* proto, int level, int* path)
	{
		for (int i = 0; i < proto->sizek; i++)
		{
			path[level] = i;
			printPath(str, level, path);
			switch (proto->k[i].tt)
			{
			case LUA_TNUMBER:
				str += stringFormat(")%f\n", float(proto->k[i].value.n));
				break;
			case LUA_TSTRING:
				str += stringFormat(")[%s]\n", svalue(&(proto->k[i])));
				break;
			default:
				str += stringFormat(") type: %s\n", lua_typename(NULL, proto->k[i].tt));
				break;
			}
		}

		for (int i = 0; i < proto->sizep; i++)
		{
			path[level] = i;
			printConsts(str, proto->p[i], level + 1, path);
		}
	}

	void ProtoEditor::drawVars(Proto* proto, vector<int>& path) {
		if (ImGui::TreeNode("vars")) {
			for (int i = 0; i < proto->sizek; i++) {
				std::string name = std::to_string(i);
				name += ")";
				auto& var = proto->k[i];
				switch (var.tt)
				{
				case LUA_TNIL:
					name += "nil";
					break;
				case LUA_TBOOLEAN:
					name += "bool: ";
					{
						char buff[16]; 
						sprintf(buff, "0x%08X", var.value.p);
						name += buff;
					}
					break;
				case LUA_TNUMBER:
					name += "n: ";
					name += std::to_string(var.value.n);
					break;
				case LUA_TSTRING:
					name += "s: ";
					name += lua_getStr(&(var));
					break;
				default:
					name += "wrong type: ";
					name += std::to_string(var.tt);
					break;
				}
				if (ImGui::BeginMenu(name.c_str())) {
					if (ImGui::MenuItem("override")) {
						path.push_back(i);
						createOverride(path, &var);
						path.pop_back();
					}
					ImGui::EndMenu();
				}
			}

			ImGui::TreePop();
		}

		for (int i = 0; i < proto->sizep; i++) {
			if (ImGui::TreeNode(std::to_string(i).c_str())) {
				path.push_back(i);
				drawVars(proto->p[i], path);
				path.pop_back();
				ImGui::TreePop();
			}
		}
	}

	void ProtoEditor::createOverride(std::vector<int> path, TValue *val) {
		ProtoManager::ProtoValue var;
		var.path = path;
		var.val = *val;
		if (var.val.tt == LUA_TSTRING) {
			var.val.value.gc = (GCObject*)malloc(512);
			lua_getStr(val);
			strcpy((char*)var.val.value.gc, (char*)val->value.gc);
		}
		master->fakeVars.push_back(var);
	}

	bool ProtoEditor::isClosing() {
		return close;
	}
}