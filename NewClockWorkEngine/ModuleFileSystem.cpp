#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

//#include <fstream>
//#include <filesystem>

#include <string.h> //used in checking file type

#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"
#include "Config.h"
#include "Importer.h"

#include "PhysFS/include/physfs.h"
#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

#include <fstream>

//linea 102 module input Importer:: load fbx -> llamaar el filesysatem y pasarle el path (dropfiledyr) para probarlo rapido


ModuleFileSystem::ModuleFileSystem(bool start_enabled)
{
	// Initialize the PhysicsFS library
	// This must be called before any other PhysicsFS function
	// This should be called prior to any attempts to change your process's current working directory
	PHYSFS_init(nullptr);

	// We only need this when compiling in debug. In Release we don't need it.
	AddNewPath(".");
	PHYSFS_setWriteDir("."); //necessary to save in own file format (working directory aka Game)
	AddNewPath("Assets");

	CreateAllLibDirectories();
}

ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
	LOG("De-init PHYSFS");
}

bool ModuleFileSystem::Init()
{
	bool ret = true;
	return ret;
}

bool ModuleFileSystem::Start()
{
	bool ret = true;
	// Determine if the PhysicsFS library is initialized, we can check it for avoid errors.
	if (PHYSFS_isInit()) {
		LOG("Asset Manager is succefully loaded");
	}
	else
		LOG("[error]Failed loading Asset Manager");

	return ret;
}

update_status ModuleFileSystem::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleFileSystem::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleFileSystem::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleFileSystem::CleanUp()
{
	bool ret = true;
	return ret;
}

uint ModuleFileSystem::Load(const char* path, char** buffer) const
{
	uint ret = 0;


	// The reading offset is set to the first byte of the file.
	// Returns a filehandle on success that we will need for the PHYSFS_fileLength
	PHYSFS_file* file = PHYSFS_openRead(path);
	if (file != nullptr)
	{
		// Check for end-of-file state on a PhysicsFS filehandle.
		if (!PHYSFS_eof(file))
		{
			// Get total length of a file in bytes
			uint lenght = PHYSFS_fileLength(file);
			*buffer = new char[lenght];
			// Read data from a PhysicsFS firehandle. Returns a number of bytes read.
			uint bytes = PHYSFS_readBytes(file, *buffer, lenght);

			if (bytes != lenght)
			{
				LOG("[error]%s", path, "ERROR: %s", PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
				ret = bytes;
		}
		else
			LOG("[error]%s", path, "ERROR: %s", PHYSFS_getLastError());
	}
	else
	{
		LOG("[error]: File '%s' doesn't exist", path);
	}
	// Close a PhysicsFS firehandle
	PHYSFS_close(file);

	return ret;
}

SDL_RWops* ModuleFileSystem::Load(const char* path)
{
	char* buffer;
	std::string newPath = NormalizePath(path);
	//std::string newPath=path;

	TransformToRelPath(newPath);

	uint bytes = Load(newPath.c_str(), &buffer);


	// Read-only memory buffer for use with RWops, retruns a pointer to a new SDL_RWops structure
	SDL_RWops* ret = SDL_RWFromConstMem(buffer, bytes);

	return ret;
}

//TODO provisional solution until we copy files into the directory
void ModuleFileSystem::TransformToRelPath(std::string& path)
{
	std::string s = "Assets";
	unsigned int splitPos = path.find(s); //file must be inside Assets directory
	if (splitPos < path.size())
	{
		path = path.substr(splitPos + s.size() + 1, path.length());
	}

}

void ModuleFileSystem::TransformToLowerCase(std::string& lowerCase)
{
	std::string newString;
	for (int i = 0; i < lowerCase.size(); i++)
	{
		newString += tolower(lowerCase[i]);
	}
	lowerCase = newString;
}

void ModuleFileSystem::SeparatePath(std::string path, std::string* newPath, std::string* file)
{
	size_t filePos = path.find_last_of("\\/");

	if (filePos < path.size())
	{
		if (newPath)* newPath = path.substr(0, filePos + 1);
		if (file)* file = path.substr(filePos + 1);
	}
	else if (path.size() > 0)
	{
		if (file)* file = path;
	}
}

void ModuleFileSystem::SeparateExtension(std::string file, std::string* ext, std::string* fileWithoutExt)
{
	size_t filePos = file.find_last_of(".");

	if (filePos < file.size())
	{
		if (ext != nullptr)
		{
		*ext = file.substr(filePos);
		}
		if (fileWithoutExt != nullptr)
		{
			*fileWithoutExt = file.substr(0, filePos);
		}
	}
}
//
//void ModuleFileSystem::SeparateExtension(std::string file, std::string* ext, std::string* filePathWithoutExt)
//{
//	size_t filePos = file.find_last_of(".");
//
//	if (filePos < file.size())
//	{
//		*ext = file.substr(filePos);
//		*filePathWithoutExt = file.substr(0, file.size() - ext->size());
//	}
//}

//normalizes '//' paths
std::string ModuleFileSystem::NormalizePath(const char* path)
{
	std::string newPath(path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}

//resource manager deals with this now
//void ModuleFileSystem::LoadAsset(char* path)
//{
//	char* buffer;
//	std::string newPath = NormalizePath(path);
//	//std::string newPath=path;
//
//	//TransformToRelPath(newPath);
//	//LOG("Loading Asset from path: %s", newPath.c_str());
//	uint size = App->fileSystem->Load((char*)newPath.c_str(), &buffer);
//
//	FileFormats thisFormat = CheckFileFormat(newPath.c_str());
//
//	switch (thisFormat)
//	{
//	case FileFormats::FBX://fbx and obj are the same, no need to call same method 2 times
//	case FileFormats::OBJ:
//		Importer::Model::ImportModel(buffer, size, newPath.c_str()); //this workas, deal with it
//		break;
//
//	case FileFormats::JSON:
//		//to be
//		break;
//
//	case FileFormats::PNG:
//	case FileFormats::JPG:
//	case FileFormats::JPEG:
//	case FileFormats::DDS:
//		Importer::Texture::LoadNewImageFromBuffer(buffer, size, newPath);
//		break;
//
//	case FileFormats::PHO:
//		Importer::LoadMeshFromPho(buffer, size, newPath);
//		//Importer::LoadMaterialFromPho(buffer, size, newPath);
//		//Importer::LoadTransformFromPho(buffer, size, newPath);
//		//Importer::LoadCameraFromPho(buffer, size, newPath);
//		break;
//	case FileFormats::UNDEFINED:
//		LOG("[error]asset from %s has no recognizable format", path);
//		break;
//	default:
//		break;
//	}
//	RELEASE_ARRAY(buffer);
//}

FileFormats ModuleFileSystem::CheckFileFormat(const char* path)
{
	FileFormats format;
	std::string newPath = path;
	std::string strFormat;
	unsigned int index = newPath.find_last_of(".");

	if (index < newPath.size())
	{
		strFormat = newPath.substr(index); //look for the last instance of a point. Format should be next
		TransformToLowerCase(strFormat);
	}

	if (strFormat.size() == 0)
	{
		// somehow no extension 
		format = FileFormats::UNDEFINED;
	}
	else
	{
		format = FileFormats::UNDEFINED;

		if (!strcmp(strFormat.c_str(), ".fbx"))
		{
			format = FileFormats::FBX;
		}
		else if (!strcmp(strFormat.c_str(), ".obj"))
		{
			format = FileFormats::OBJ;
		}
		else if (!strcmp(strFormat.c_str(), ".png"))
		{
			format = FileFormats::PNG;
		}
		else if (!strcmp(strFormat.c_str(), ".dds"))
		{
			format = FileFormats::DDS;
		}
		else if (!strcmp(strFormat.c_str(), ".jpg"))
		{
			format = FileFormats::JPG;
		}
		else if (!strcmp(strFormat.c_str(), ".jpeg"))
		{
			format = FileFormats::JPEG;
		}
		else if (!strcmp(strFormat.c_str(), ".json"))
		{
			format = FileFormats::JSON;
		}
		else if (!strcmp(strFormat.c_str(), ".tga"))
		{
			format = FileFormats::TGA;
		}
		else if (!strcmp(strFormat.c_str(), ".pho"))
		{
			format = FileFormats::PHO;
		}
	}
	return format;
}

unsigned long ModuleFileSystem::GetLastModTimeFromPath(const char* file)
{
	return PHYSFS_getLastModTime(file);
}


// Save a whole buffer to disk
unsigned int ModuleFileSystem::SavePHO(const char* file, const void* buffer, unsigned int size)
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;


	PHYSFS_file* fs_file = PHYSFS_openWrite(file);

	if (!overwrite)
	{
		//uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
	}

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG("[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (overwrite == true)
			{
				LOG("File [%s] overwritten with %u bytes", file, size);
			}
			else
			{
				LOG("New file created [%s] of %u bytes", file, size);
			}
			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("[error] File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG("[error] File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}

bool ModuleFileSystem::IsFileDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;//TODO, deprecated funct? use PHYSFS_stat()?
}

bool ModuleFileSystem::CreateNewDirectory(const char* directory)
{
	bool ret = false;

	if (!IsFileDirectory(directory))
	{
		PHYSFS_mkdir(directory);
		ret = true;
	}

	return ret;
}

void ModuleFileSystem::CreateAllLibDirectories()
{
	CreateNewDirectory(LIB_PATH);
	CreateNewDirectory(MESH_PATH);
	CreateNewDirectory(MATERIAL_PATH);
	CreateNewDirectory(TEXTURE_PATH);
	CreateNewDirectory(SCENE_PATH);
	CreateNewDirectory(MODEL_PATH);

}

bool ModuleFileSystem::AddNewPath(const char* newPath)
{
	bool ret = false;

	if (PHYSFS_mount(newPath, nullptr, 1) == 0)
	{
		LOG("[error] Path could not be added to the file system: %s", PHYSFS_getLastError());
	}
	else
	{
		ret = true;
	}

	return ret;
}

bool ModuleFileSystem::GetDirFiles(const char* dir, std::vector<std::string>& fileList, std::vector<std::string>& dirList)
{
	bool ret = false;

	if (DoesFileExist(dir))
	{
		char** rc = PHYSFS_enumerateFiles(dir);
		char** iter;

		std::string directory = dir;

		for (iter = rc; *iter != nullptr; iter++)
		{
			if (PHYSFS_isDirectory((directory + *iter).c_str()))
				dirList.push_back(*iter);
			else
				fileList.push_back(*iter);
		}

		PHYSFS_freeList(rc);


		ret = true;
	}

	return ret;
}

bool ModuleFileSystem::DoesFileExist(const char* file)
{
	return PHYSFS_exists(file) != 0;
}

bool ModuleFileSystem::DeleteFromAssetsAndLibs(const char* assetPath, bool isMeta)
{
	bool ret = false;

	//Deleting all info regarding the asset:

	LOG("DELETING %s", assetPath);

	//Does the asset exist?


	if (PHYSFS_exists(assetPath) || isMeta)
	{
		std::string metaPath = assetPath;
		if (!isMeta)
		{
			//Delete it
			if (PHYSFS_delete(assetPath) == 0) //zero on error. success in everything else
			{
				LOG("[error] Tried to delete %s and found it, but was unable to delete. PHYSFS gives this error:", assetPath);
				LOG("%s", PHYSFS_getLastError());
			}
			else
			{
				LOG("%s DELETED from assets", assetPath);
			}

			//Does the asset have a meta file?

			metaPath += ".meta";
		}
		if (PHYSFS_exists(metaPath.c_str()))
		{
			//Store some info from meta to delete the lib file later
			char* auxB = "";
			unsigned int size = Load(metaPath.c_str(), &auxB);
			Config metaFile(auxB);
			int UID = metaFile.GetNumber("ID");

			//Delete the meta
			if (PHYSFS_delete(metaPath.c_str()) == 0)
			{
				LOG("[error] Tried to delete %s and found it, but was unable to delete. PHYSFS gives this error:", metaPath.c_str());
				LOG("%s", PHYSFS_getLastError());
			}
			else
			{
				LOG("%s DELETED from assets", metaPath.c_str());
			}


			//Is the asset imported in lib?
			std::string libPath = "";
			App->rManager->FindFileRecursively(std::to_string(UID), LIB_PATH, libPath); //try to find the asset in lib searching its UID we got from the meta


			if (libPath!="" && PHYSFS_exists(libPath.c_str()))
			{
				//Delete it

				if (PHYSFS_delete(libPath.c_str()) == 0) //zero on error. success in everything else
				{
					LOG("[error] Tried to delete %s and found it, but was unable to delete. PHYSFS gives this error:", libPath.c_str());
					LOG("%s", PHYSFS_getLastError());
				}
				else
				{
					LOG("%s DELETED from lib", libPath.c_str());
				}

				//Do we have it in memory?

				if (App->rManager->DeleteItemFromResourcesMap(UID))
				{
					LOG("DELETED %s from memory", libPath.c_str());
				}
				else
				{
					LOG("Could not delete %s from memory because it was never loaded in the first place", libPath.c_str());
				}
			}
			else
			{
				LOG("[error] Tried to delete the lib file %s but it was not found", libPath.c_str());
			}
		}
		else
		{
			LOG("[error] Tried to delete the meta file %s but it was not found", metaPath.c_str());
		}
	}
	else
	{
		LOG("[error] Tried to delete %s but was unable to find such file", assetPath);
	}
	return ret;
}

bool ModuleFileSystem::DeleteTemporalScene(char* sceneBuffer)
{
	return false;
}

void ModuleFileSystem::FindFileInDirectory(std::string fileName,std::string currDir, std::string& fullPath,bool excludeMeta)
{
	std::vector<std::string>files;
	std::vector<std::string>dirs;
	
	if (!App->fileSystem->GetDirFiles(currDir.c_str(), files, dirs))
	{
		LOG("[error] Trying to acces an invalid directory: %s", currDir.c_str());
		return; //if directory doesn't exist return, just for safety
	}


	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;
		std::string absPath = currDir + str;

		std::string extension;

		App->fileSystem->SeparateExtension(str, &extension,nullptr);

		if (excludeMeta)
		{
			if (extension == ".meta")continue;
		}
		if (str == (fileName + extension))
		{
			fullPath = absPath;
			return;
		}
	}

	//for every directory in the current directory, search
	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		const std::string& str = *it;
		FindFileInDirectory(fileName, currDir + str + "/", fullPath);
		if (fullPath != "")
		{
			return;
		}
	}


}

//WARNING paths do not accept caracters such as accents in the original file path and the destination folder, consider renaming assets before importing
bool ModuleFileSystem::DuplicateFile(const char* originalFilePath, const char* destinationFolder, std::string& relativePath)
{
	std::string newFile;//only the file name
	std::string newExtension;//only the extension
	SeparatePath(originalFilePath, nullptr, &newFile);
	SeparateExtension(newFile, &newExtension, &newFile);

	relativePath = relativePath.append(destinationFolder).append("/") + newFile + newExtension; //construct destination path (aka. Assets/samirkebab.png)
	std::string finalPath = std::string(*PHYSFS_getSearchPath()).append("/") + relativePath;//search path-> ./Assets/...relative path

	std::ifstream src;
	src.open(originalFilePath, std::ios::binary);
	bool srcOpen = src.is_open();

	if (!srcOpen)
	{
		src.close();
		LOG("[error] File couldn't be duplicated, we couldn't open source directory with path:\n %s", originalFilePath);
		return false;
	}

	std::ofstream  dst(finalPath, std::ios::binary);
	bool dstOpen = dst.is_open();
	if (!dstOpen)
	{
		dst.close();
		LOG("[error] File couldn't be duplicated, we couldn't open destination directory with path:\n %s", finalPath);
		return false;
	}

	dst << src.rdbuf();

	src.close();
	dst.close();

	return true;
}


