#ifndef __RESOURCES_H__
#define __RESOURCES_H__
#include <string>

//#include "Config.h"

//class Config;

enum class ResourceType {
	TEXTURE,
	MESH,
	SCENE,
	MODEL,
	UNKNOWN
};

class Resource
{
public:
	Resource(unsigned int UID,ResourceType type);
	virtual ~Resource();

	ResourceType GetType()const;
	unsigned int GetUID()const;
	void SetUID(int newUID);

	//Reminder GetAssetFile().c_str() has to be assigned to a string variable if you want to work with it, otherwise it gets deleted
	std::string GetAssetFile() const;
	std::string GetLibraryFile() const;
	std::string GetName() const;
	void SetAssetPath(std::string newPath);
	void SetLibPath(std::string newPath);
	void SetName(std::string newName);

	virtual bool UnloadFromMemory();

	bool IsLoadedInMemory()const;
	void SetIsLoadedTo(bool state);
public:
	unsigned int referenceCount;


protected:
	bool isLoaded;
	unsigned int uid;
	ResourceType type;

	std::string assetsFile;
	std::string libraryFile;
	std::string name;


};


#endif // !__RESOURCES_H__
