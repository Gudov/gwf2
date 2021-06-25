#include "protoManager.h"

#include <fstream>
#include <streambuf>
#include <Windows.h>
#include <unordered_map>

#include "json.hpp"

#include "console.h"

using nlohmann::json;
using std::string;
using std::vector;

EXTERN_C_START
static const char* lua_getStr(TValue *val) {
	return svalue(val);
}
EXTERN_C_END

namespace ProtoManager {
	std::unordered_map<std::string, Proto*> protos;
	std::unordered_map<std::string, vector<ProtoMaster*>> protoMasters;

	vector<ProtoMaster*> getProtoMasters(std::string path, std::string name) {
		path += "/";
		path += name;
		auto master = protoMasters.find(path);
		if (master == protoMasters.end()) {
			return {};
		}
		return (*master).second;
	}

	void addProtoMaster(std::string path, std::string name, ProtoMaster* master) {
		path += "/";
		path += name;

		auto masterFind = protoMasters.find(path);
		if (masterFind == protoMasters.end()) {
			protoMasters[path] = std::vector<ProtoMaster*>();
		}

		protoMasters[path].push_back(master);
	}

	std::unordered_map<string, vector<ProtoMaster*>> *getAllProtoMasters() {
		return &protoMasters;
	}

	void proccedProto(std::string path, std::string name, Proto* proto) {
		saveProto(path, name, proto);
		vector<ProtoMaster*> masters = getProtoMasters(path, name);
		for (auto master : masters) {
			if (master->enabled) {
				applyProtoMaster(proto, master);
			}
		}
	}

	void saveProto(std::string path, std::string name, Proto* proto) {
		path += "/";
		path += name;
		protos[path] = proto;
	}

	Proto* getProto(std::string path, std::string name) {
		path += "/";
		path += name;
		auto master = protos.find(path);
		if (master == protos.end()) {
			return nullptr;
		}
		return (*master).second;
	}

	bool applyProtoMaster(Proto* proto, ProtoMaster* master) {
		for (auto var : master->fakeVars) {
			Proto* p = proto;
			size_t pos = 0;
			while (var.path.size() < pos) {
				int i = var.path[pos];
				if (proto->sizep >= i) {
					return false;
				}
				p = p->p[i];
				pos++;
			}

			int i = var.path[pos];
			if (p->sizek >= i) {
				return false;
			}
			auto& value = p->k[i];
			if (value.tt != var.val.tt) {
				return false;
			}

			value.value = var.val.value;
		}
	}

	void loadAllProtos(std::string folder) {
		folder.append("\\*.json");
		WIN32_FIND_DATA data;
		HANDLE hFind;
		if ((hFind = FindFirstFile(folder.c_str(), &data)) == INVALID_HANDLE_VALUE) {
			return;
		}

		do {
			ProtoMaster *master = loadProtoMaster(data.cFileName);
			if (master) {
				addProtoMaster(master->filePath, master->fileName, master);
			}
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}

	void saveProtoMaster(ProtoMaster* master, std::string file) {
		if (master == nullptr) {
			return;
		}
		json masterJson;

		masterJson["name"] = master->name;
		masterJson["fileName"] = master->fileName;
		masterJson["filePath"] = master->filePath;

		masterJson["vars"] = json::array();
		for (auto& var : master->fakeVars) {
			json varJson;

			varJson["path"] = json::array();
			for (auto path : var.path) {
				varJson["path"].push_back(path);
			}

			if (var.val.tt == LUA_TSTRING) {
				varJson["type"] = "string";
				varJson["value"] = lua_getStr(&(var.val));
			} else if (var.val.tt == LUA_TNUMBER) {
				varJson["type"] = "number";
				varJson["value"] = var.val.value.n;
			} else {
				varJson["type"] = "fuck";
			}

			masterJson["vars"].push_back(varJson);
		}

		std::string data = masterJson.dump();
		FILE* f = fopen(file.c_str(), "w");
		fwrite(data.c_str(), 1, data.length(), f);
		fclose(f);
	}

	static bool loadJson(std::string filePath, json &const_json) {
		std::ifstream file(filePath);
		std::string str((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		try {
			const_json = json::parse(str);
		}
		catch (const std::exception&) {
			fprintf(
				Console::getInstance()->fpout,
				"failed to load config file...\n  %s\nusing empty\n",
				filePath.c_str());
			fflush(Console::getInstance()->fpout);
			return false;
		}

		return true;
	}

	ProtoMaster* loadProtoMaster(std::string filePath) {
		json masterJson;
		if (!loadJson(filePath, masterJson)) {
			return nullptr;
		}

		ProtoMaster *master = new ProtoMaster();
		master->name = masterJson["name"].get<string>();
		master->fileName = masterJson["fileName"].get<string>();
		master->filePath = masterJson["filePath"].get<string>();
		
		for (auto& element : masterJson["vars"]) {
			ProtoValue var;

			std::string type = element["type"].get<std::string>();

			for (auto& path : element["path"]) {
				var.path.push_back(path.get<int>());
			}

			auto value = element["value"];

			if (type == "string") {
				char* buff = (char*)malloc(512);
				strcpy(buff, (value.get<std::string>()).c_str());
				setsvalue(L, &(var.val), buff);
			} else if (type == "number") {
				setnvalue(&(var.val), value.get<float>())
			}

			master->fakeVars.push_back(var);
		}

		master->enabled = false;

		return master;
	}

	void deleteProtoMaster(std::string file) {
		DeleteFileA(file.c_str());
	}
}
