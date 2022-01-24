#ifndef __MODULEFILESYSTEModuleH__
#define __MODULEFILESYSTEModuleH__
#include "Module.h"
//#include "Globals.h"
#include "SDL/include/SDL_rwops.h"
#include <vector>
#include <string>

enum class FileFormats
{
	UNDEFINED,

	FBX,
	JSON,
	OBJ,
	PNG,
	JPG,
	JPEG,
	DDS,
	TGA,
	PHO //<3
};


class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	~ModuleFileSystem();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	// Return the bytes of a PhysFS filehandle
	unsigned int Load(const char* path, char** buffer) const;

	// Allows you to use pointers to memory instead of files or things such as images or samples
	SDL_RWops* Load(const char* path);

	void TransformToRelPath(std::string& path);
	void TransformToLowerCase(std::string& lowerCase);

	void SeparatePath(std::string path, std::string* newPath, std::string* file);
	void SeparateExtension(std::string file, std::string* ext,std::string* filePathWithoutExt);

	std::string NormalizePath(const char* path);

	FileFormats CheckFileFormat(const char* path);

	unsigned long GetLastModTimeFromPath(const char* filename);

	// PHO
	unsigned int SavePHO(const char* file, const void* buffer, unsigned int size);

	//checks if a certain file is a directory instead
	bool IsFileDirectory(const char* file)const;
	
	//returns true if directory has been created, otherwise(if it already exists) returns false
	bool CreateNewDirectory(const char* directory);
	
	//creates all Library folder structure
	void CreateAllLibDirectories();
	
	//Adds a new path to the search path, returns true on success
	bool AddNewPath(const char* newPath);
	
	//Returns a vector listing all files and folders in a specific directory, also returns false if specified directory doesn't exist
	bool GetDirFiles(const char* dir, std::vector<std::string> &fileList, std::vector<std::string>& dirList);
	
	//Returns true if a flie exists, otherwise returns false
	bool DoesFileExist(const char* file);

	//deleting all references from file inspector on "delete" button
	bool DeleteFromAssetsAndLibs(const char* assetPath,bool isMeta=false);//TODO shouldn't this function be on rManager?

	//deleting temporal scenes
	bool DeleteTemporalScene(char * sceneBuffer);

	void FindFileInDirectory(std::string fileName,std::string currDir, std::string& fullPath,bool excludeMeta=true);

	bool DuplicateFile(const char* originalFilePath, const char* destinationFolder, std::string& relativePath);

};

#endif // __MODULEFILESYSTEModuleH__
