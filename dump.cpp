#include "dump.h"
#include <vector>
#include <Windows.h>
#include <string>


using std::string;
using std::vector;

namespace DumpManger {
	std::vector<std::string> strSplit(const std::string& str, const std::string& delim) {
		std::vector<std::string> tokens;
		size_t prev = 0, pos = 0;
		do
		{
			pos = str.find(delim, prev);
			if (pos == std::string::npos) pos = str.length();
			std::string token = str.substr(prev, pos - prev);
			if (!token.empty()) tokens.push_back(token);
			prev = pos + delim.length();
		} while (pos < str.length() && prev < str.length());
		return tokens;
	}

	DumpedData* root = nullptr;

	DumpedData* getOrCreateFolder(const vector<string> &path) {
		DumpedData* root = getRoot();
		DumpedData* folder = root;
		for (auto folderName : path) {
			auto &map = std::get<DumpedData::Map>(folder->data);
			if (!map.contains(folderName)) {
				map[folderName] = createFolder();
			}
			folder = map[folderName];
		}

		return folder;
	}

	BOOL DirectoryExists(LPCTSTR szPath)
	{
		DWORD dwAttrib = GetFileAttributes(szPath);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	void createDirectoryRecursively(std::string path)
	{
		unsigned int pos = 0;
		do
		{
			pos = path.find_first_of("\\/", pos + 1);
			CreateDirectory((LPCSTR)path.substr(0, pos).c_str(), NULL);
		} while (pos != std::string::npos && pos < 255);
	}

	std::string dumpPath = "C:\\war3\\dump";
}

DumpedData* DumpManger::getRoot() {
	if (!root) {
		root = new DumpedData(true);
		root->isFolder = true;
		root->data = DumpedData::Map();
	}
	return root;
}

void DumpManger::clear() {
	DumpedData* oldRoot = root;
	root = nullptr;
	delete oldRoot;
}

void DumpManger::addData(std::string strPath, std::string name, uint8_t* data, size_t size) {
	vector<string> path = strSplit(strPath, "/");

	DumpedData* folder = getOrCreateFolder(path);
	auto &map = std::get<DumpedData::Map>(folder->data);
	if (map.contains(name)) {
		delete map[name];
	}

	uint8_t* dataCopy = new uint8_t[size];
	memcpy(dataCopy, data, size);
	map[name] = createFile(dataCopy, size);
}

void DumpManger::saveFile(std::string path, std::string name, DumpedData* file) {
	if (DirectoryExists(path.c_str()) == FALSE)
		createDirectoryRecursively(path);

	char full_path[255];
	sprintf(full_path, "%s\\%s", path.c_str(), name.c_str());
	FILE* f = fopen(full_path, "wb");
	auto bin = std::get<DumpedData::File>(file->data);
	fwrite(bin.first, bin.second, 1, f);
	fclose(f);
}

void DumpManger::saveFolder(std::string prevPath, DumpedData *folder) {
	if (folder->isFolder) {
		auto &map = std::get<DumpedData::Map>(folder->data);
		for (auto v : map) {
			if (v.second->isFolder) {
				saveFolder(prevPath + "\\" + v.first, v.second);
			} else {
				saveFile(prevPath, v.first, v.second);
			}
		}
	}
}

void DumpManger::saveAll() {
	DumpedData* base = getRoot();
	try {
		saveFolder(dumpPath, base);
	}
	catch (const std::bad_variant_access&) {}
}

void DumpManger::setFolder(std::string path) {
	dumpPath = path;
}

DumpedData* DumpManger::createFile(uint8_t* data, size_t size) {
	DumpedData* folder = new DumpedData(false);
	folder->isFolder = false;
	folder->data = std::pair<uint8_t*, size_t>(data, size);
	return folder;
}

DumpedData* DumpManger::createFolder() {
	DumpedData *folder = new DumpedData(true);
	folder->isFolder = true;
	folder->data = DumpedData::Map();
	return folder;
}

DumpedData::DumpedData(bool isFolder) {
	this->isFolder = isFolder;
}

DumpedData::~DumpedData() {
	try {
		if (this->isFolder) {
			auto map = std::get<DumpedData::Map>(data);
			for (auto v : map) {
				delete v.second;
			}
		} else {
			auto pair = std::get<std::pair<uint8_t*, size_t>>(data);
			delete pair.first;
		}
	}
	catch (const std::bad_variant_access&) {}
}
