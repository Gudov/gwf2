#pragma once

extern "C"
{
#include "lua/lua.h"
#include "lua/lstate.h"
#include "lua/lapi.h"
}

#include <vector>
#include <list>
#include <string>

#include "protoManager.h"

using ProtoManager::ProtoMaster;

namespace GUI {
	class ProtoEditor {
	public:
		static void drawAll();

		ProtoEditor(std::string path, std::string name, Proto* p);
		ProtoEditor(std::string path, std::string name, Proto* p, ProtoMaster* master);

		void draw();
		bool isClosing();
	private:
		void drawChanges();
		void drawVars(Proto *p, std::vector<int> &path);
		void createOverride(std::vector<int> path, TValue* val);
		void printConsts(std::string &str, Proto* proto, int level, int* path);

		char masterName[256];
		std::string path;
		std::string name;
		Proto* p;
		ProtoMaster* master;
		
		bool newFile = true;

		bool windowShow = true;
		bool close = false;
	};

	extern std::list<ProtoEditor*> editors;
}