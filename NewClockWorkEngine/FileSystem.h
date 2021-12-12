#pragma once

#include "Module.h"
#include <vector>
#include <string>
#include "Globals.h"

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops* rw);

struct aiFileIO;

class Config;
struct PathNode;

class FileSystem : public Module
{
public:

	FileSystem(bool start_enabled = true);
	~FileSystem();


	bool Init() override;

	bool CleanUp() override;

	void CreateLibraryDirectories();

	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) const;
	bool HasExtension(const char* path) const;
	bool HasExtension(const char* path, std::string extension) const;
	bool HasExtension(const char* path, std::vector<std::string> extensions) const;
	bool DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);
	bool Remove(const char* file);

	const char* GetWriteDir() const;

	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	void GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const;
	void GetRealDir(const char* path, std::string& output) const;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	PathNode GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr) const;

	std::string GetPathRelativeToAssets(const char* originalPath) const;
	std::string NormalizePath(const char* path) const;
	std::string GetUniqueName(const char* path, const char* name) const;

	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;

	uint64 GetLastModTime(const char* filename);
};


