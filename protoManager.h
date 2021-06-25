#pragma once
extern "C"
{
	#include "lua/lua.h"
	#include "lua/lstate.h"
	#include "lua/lapi.h"
}

#include <string>
#include <vector>
#include <unordered_map>

namespace ProtoManager {
	class ProtoValue {
	public:
		TValue val;
		std::vector<int> path;
	};

	class ProtoMaster {
	public:
		std::string name;
		std::string fileName;
		std::string filePath;
		bool enabled;
		std::vector<ProtoValue> fakeVars;
	};

	const std::string masterPath = "C:\\war3\\proto";

	std::vector<ProtoMaster*> getProtoMasters(std::string path, std::string name);
	void addProtoMaster(std::string path, std::string name, ProtoMaster* master);
	std::unordered_map<std::string, std::vector<ProtoMaster*>> *getAllProtoMasters();

	void saveProto(std::string path, std::string name, Proto* proto);
	Proto* getProto(std::string path, std::string name);

	void proccedProto(std::string path, std::string name, Proto* proto);
	bool applyProtoMaster(Proto* proto, ProtoMaster* master);

	void loadAllProtos(std::string folder);
	void saveProtoMaster(ProtoMaster* master, std::string file);
	ProtoMaster* loadProtoMaster(std::string file);
	void deleteProtoMaster(std::string file);
};