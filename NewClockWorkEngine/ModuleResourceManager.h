#pragma once

#include "Module.h"
#include "Globals.h"

#include <string>
#include <vector>
#include <map>

class Resource;
enum class ResourceType;

struct ActiveResources
{
	std::vector<Resource*> meshes;
	std::vector<Resource*> textures;
	std::vector<Resource*> scenes;
	std::vector<Resource*> models;
};



class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(bool start_enabled = true);
	~ModuleResourceManager();
	bool Init();
	bool Start();
	update_status PreUpdate(float dt)override;
	//update_status Update(float dt)override;
	//update_status PostUpdate(float dt)override;
	bool CleanUp();

	Resource* ImportNewFile(const char* newAssetFile);
	Resource* ReImportExistingFile(const char* newAssetFile,unsigned int uid);
	//given a uid the system checks for any lib file with that uid
	void FindFileRecursively(std::string uid, std::string currDir, std::string& foundFile);//Deprecated??

	void GenerateMetaFile(Resource* res);

	//this is used when an object requests a new resource, if you want to load it to scene use TryLoadResIntoScene()
	Resource* RequestNewResource(unsigned int uid);
	//This doesn't add to the resource count, we use it during the execution
	Resource* RequestExistingResource(unsigned int uid);
	void StopUsingResource(unsigned int uid);
	//this method will try to load the requested resource directly into scene (in the case of meshes & images it will put the mesh/image into the selected object if any), it returns false on failure
	bool TryLoadResIntoScene(unsigned int uid);

	//getAll=true will get you all resources imported, the ones active and the ones that are not active
	ActiveResources GetActiveResources(bool getAll = false);
	//if creatinga  new resource do not use the last parameter, it is only used when loading resources
	Resource* CreateNewResource(const char* assetsFile, ResourceType type,unsigned int existingID=0);//we need this for Import 
	//Manages importing of an asset file (whether it has to import, create lib file, update from meta, etc)
	Resource* ManageAssetUpdate(const char* newAssetFile);

	bool DeleteItemFromResourcesMap(unsigned int UID);

	//finds a certain resource even if it is not in memory
	Resource* FindResInMemory(unsigned int UID);
	//only base paremeters is only used for the moldels, as they have other resource dependencies, use this bool as true when you only want to load the base resources into memory and not the actual Model
	void LoadResourceIntoMem(Resource* res, bool onlyBase = false);


	void GetAllResourcesOfType(ResourceType type, std::vector<Resource*>& ret);

private:
	void LoadAllAssets();

	
	void LoadAssetsRecursively(std::string dir);

	void LoadAssetsFromDir(std::string dir);




	std::string GenLibPath(Resource& res);
	std::string GenNameFromPath(Resource& res);
	//Returns resource type from assets path
	ResourceType ResourceTypeFromPath(std::string path);

	bool ReleaseSingleResource(unsigned int uid);

	void SaveResource(Resource& r);

private:
	float checkTimer;
public:
	bool haveToReload;

	std::map<unsigned int, Resource*> resources;
};

