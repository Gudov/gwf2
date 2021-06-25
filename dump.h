#pragma once
#include <string>
#include <variant>
#include <unordered_map>
#include "protoManager.h"

struct DumpedData {
	DumpedData(bool);
	~DumpedData();
	bool isFolder;// or file
	bool isOpenedGui = false;

	using Map = std::unordered_map<std::string, DumpedData*>;
	struct File {
		uint8_t* data;
		size_t len;
		std::string path;
		std::string name;
		Proto* p;
	};//std::pair<uint8_t*, size_t>;

	std::variant<File, Map> data;
};

namespace DumpManger {
	DumpedData* getRoot();
	void clear();

	void addData(std::string path, std::string name, uint8_t* data, size_t size, Proto *p);

	void saveFile(std::string path, std::string name, DumpedData *file);
	void saveFolder(std::string prevPath, DumpedData* folder);
	void saveAll();

	void setFolder(std::string path);

	DumpedData* createFile(std::string path, std::string name, uint8_t* data, size_t size, Proto *p);
	DumpedData* createFolder();
}