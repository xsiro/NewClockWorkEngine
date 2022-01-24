#include "ModuleResourceManager.h"
#include "Globals.h"
#include "Application.h"
#include "Resources.h"
#include "Config.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"
#include <string>
#include "Importer.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"
#include "ModuleTransform.h"
#include "ModuleMesh.h"
#include "ModuleMaterial.h"
ModuleResourceManager::ModuleResourceManager(bool start_enabled) :Module(start_enabled), checkTimer(0.0f), haveToReload(false)
{
}

ModuleResourceManager::~ModuleResourceManager()
{
}

bool ModuleResourceManager::Init()
{
	return true;
}

bool ModuleResourceManager::Start()
{

	LoadAllAssets();

	std::string streetEnvironmentPath = "Assets/street/Street environment_V01.FBX";
	Resource* r = App->rManager->ManageAssetUpdate(streetEnvironmentPath.c_str());
	if (r != nullptr)
	{
		App->rManager->RequestNewResource(r->GetUID());
	}



	//TODO this is hardcoded to make the street environment look good fix that (Importing options maybe¿??)
	if (!App->scene->root->children.empty())//this is just for the initial street environment mesh, it seems to be off by 90 degrees from export
	{
		ModuleTransform* t = App->scene->root->children[0]->GetComponent<ModuleTransform>();

		float4x4 initialMat = float4x4::RotateX(DegToRad(-90.0f));

		t->SetGlobalTransform(initialMat);
	}

	return true;
}

update_status ModuleResourceManager::PreUpdate(float dt)
{
	checkTimer += dt;
	if (App->GetGameState() == GameStateEnum::STOPPED && (checkTimer >= 60.0f || haveToReload == true))//we only check assets when the engine is not in runtime
	{
		haveToReload = false;
		checkTimer = 0.0f;

		LoadAllAssets();


	}
	return update_status::UPDATE_CONTINUE;
}

//update_status M_ResourceManager::Update(float dt)
//{
//	return update_status::UPDATE_CONTINUE;
//}
//
//update_status M_ResourceManager::PostUpdate(float dt)
//{
//	return update_status::UPDATE_CONTINUE;
//}

bool ModuleResourceManager::CleanUp()
{
	return true;
}

Resource* ModuleResourceManager::ImportNewFile(const char* newAssetFile)
{
	//if it has no meta, import normally (create resource in lib + create . meta in assets)
	Resource* res = nullptr;
	ResourceType resType = ResourceTypeFromPath(newAssetFile);
	if (resType != ResourceType::UNKNOWN)
	{

		res = CreateNewResource(newAssetFile, resType);

		char* buffer;
		uint size = App->fileSystem->Load(newAssetFile, &buffer);
		if (size != 0)
		{


			//Import here->TODO->  method is given a buffer + a resource and saves the imported resource into the given one
			switch (resType)
			{
			case ResourceType::TEXTURE:
				Importer::Texture::ImportImage(buffer, size, *res);
				break;
			case ResourceType::MESH:
				//how tf did you get here
				break;
			case ResourceType::SCENE:
				break;
			case ResourceType::MODEL:
				Importer::Model::ImportModel(buffer, size, newAssetFile, res);
				break;
			case ResourceType::UNKNOWN:
			default:
				break;
			}

			//We then save the resource (TODO) -> create resource in lib + create . meta in assets
			SaveResource(*res);

			GenerateMetaFile(res);

			res->referenceCount = 0;
			if (res->IsLoadedInMemory())
			{
				res->UnloadFromMemory();
			}

			RELEASE_ARRAY(buffer);
		}
	}


	//after we are done using it, we unload the resource TODO

	return res;
}

Resource* ModuleResourceManager::ReImportExistingFile(const char* newAssetFile, unsigned int uid)
{
	Resource* res = nullptr;
	//if it has no meta, import normally (create resource in lib + create . meta in assets)

	ResourceType resType = ResourceTypeFromPath(newAssetFile);
	if (resType != ResourceType::UNKNOWN)
	{

		res = CreateNewResource(newAssetFile, resType, uid);

		char* buffer;
		uint size = App->fileSystem->Load(newAssetFile, &buffer);
		//Import here->TODO->  method is given a buffer + a resource and saves the imported resource into the given one
		switch (resType)
		{
		case ResourceType::TEXTURE:
			Importer::Texture::ImportImage(buffer, size, *res);
			break;
		case ResourceType::MESH:
			//how tf did you get here
			break;
		case ResourceType::SCENE:
			break;
		case ResourceType::MODEL:
			Importer::Model::ImportModel(buffer, size, newAssetFile, res);
			break;
		case ResourceType::UNKNOWN:
		default:
			break;
		}

		//We then save the resource (TODO) -> create resource in lib + create . meta in assets
		SaveResource(*res);

		GenerateMetaFile(res);

		res->referenceCount = 0;
		if (res->IsLoadedInMemory())
		{
			res->UnloadFromMemory();
		}

		RELEASE_ARRAY(buffer);
	}




	return res;
}


void ModuleResourceManager::FindFileRecursively(std::string uid, std::string currDir, std::string& foundFile)
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

		App->fileSystem->SeparateExtension(str, &extension, nullptr);

		if (str == (uid + extension))
		{
			foundFile = absPath;
			return;
		}
	}

	//for every directory in the current directory, search
	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		const std::string& str = *it;
		FindFileRecursively(uid, currDir + str + "/", foundFile);
		if (foundFile != "")
		{
			return;
		}
	}
}

void ModuleResourceManager::GenerateMetaFile(Resource* res)
{
	char* bufferToFill = "";
	std::string auxP = (res->GetAssetFile());
	App->fileSystem->TransformToRelPath(auxP);
	const char* assetPath = auxP.c_str();

	std::string metaExtension = ".meta";

	//fill the buffer with the necessary info
	Config file;
	file.SetNumber("ID", res->GetUID());
	unsigned long lastModDate = App->fileSystem->GetLastModTimeFromPath(assetPath);
	file.SetNumber("ModDate", lastModDate);
	//TODO: import configs go here, if we're adding that into the future
	unsigned int size = file.Serialize(&bufferToFill);

	
	std::string assets = "Assets/";
	std::string AuxPath = assets + assetPath + metaExtension;

	//make physfs save the file
	App->fileSystem->SavePHO(AuxPath.c_str(), bufferToFill, size);
	RELEASE_ARRAY(bufferToFill);

}

Resource* ModuleResourceManager::RequestNewResource(unsigned int uid)
{
	//Find if the resource is already loaded or imported
	std::map<unsigned int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		if (!it->second->IsLoadedInMemory())//if it is not loaded
		{
			//Find the library file and load the custom file format
			LoadResourceIntoMem(it->second);

		}
		if (it->second->GetType() != ResourceType::MODEL)
		{
			it->second->referenceCount++;
		}
		return it->second;
	}
	LOG("[error] The requested resource with ID: %i, doesn't exist", uid);
	return nullptr;//the file doesn't exist in lib :c
}

Resource* ModuleResourceManager::RequestExistingResource(unsigned int uid)
{
	Resource* ret = nullptr;
	std::map<unsigned int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end() && it->second->IsLoadedInMemory())
	{
		ret = it->second;

	}
	return ret;
}

void ModuleResourceManager::StopUsingResource(unsigned int uid)
{
	//Find if the resource is already loaded or imported
	std::map<unsigned int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end() && it->second->IsLoadedInMemory())
	{
		it->second->referenceCount--;
		if (it->second->referenceCount <= 0)
		{
			it->second->referenceCount = 0;
			it->second->UnloadFromMemory();
		}

	}
}

bool ModuleResourceManager::TryLoadResIntoScene(unsigned int uid)
{

	//Find if the resource is already loaded or imported
	std::map<unsigned int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{

		ResourceType resType = it->second->GetType();
		if (resType != ResourceType::MODEL)
		{

			if (App->scene->selectedGameObjs.empty())
			{
				LOG("[warning] This resource cannot be loaded if there is no selected object to apply the resource to it");
				return false;
			}

			bool ret = false;
			GameObject* currObj = App->scene->selectedGameObjs.back();

			switch (resType)
			{
			case ResourceType::TEXTURE:
			{
				ModuleMaterial* mat = currObj->GetComponent<ModuleMaterial>();
				//if it has the component, just change its resource otherwise create the component first
				if (!mat)
				{
					mat = (ModuleMaterial*)currObj->CreateComponent(ComponentType::MATERIAL);
				}
				mat->SetNewResource(uid);
				ret = true;
			}
			break;
			case ResourceType::MESH:
			{
				ModuleMesh* mesh = currObj->GetComponent<ModuleMesh>();
				
				if (!mesh)
				{
					mesh = (ModuleMesh*)currObj->CreateComponent(ComponentType::MESH);
				}
				mesh->SetNewResource(uid);
				ret = true;
			}
			break;
			}

			return ret;
		}
		else
		{
			LoadResourceIntoMem(it->second);
			return true;
		}

	}

	LOG("[error] The requested resource with ID: %i, doesn't exist", uid);
	return false;
}

ActiveResources ModuleResourceManager::GetActiveResources(bool getAll)
{
	ActiveResources res;

	std::map<unsigned int, Resource*>::iterator it = resources.begin();

	for (it; it != resources.end(); it++)
	{
		if (getAll || it->second->IsLoadedInMemory())
		{

			ResourceType type = it->second->GetType();
			switch (type)
			{
			case ResourceType::TEXTURE:
				res.textures.push_back(it->second);
				break;
			case ResourceType::MESH:
				res.meshes.push_back(it->second);
				break;
			case ResourceType::SCENE:
				res.scenes.push_back(it->second);
				break;
			case ResourceType::MODEL:
				res.models.push_back(it->second);
				break;
			case ResourceType::UNKNOWN:
			default:
				LOG("Unknown active resource with id: %i", it->second->GetUID());
				break;
			}
		}
	}


	return res;
}

void ModuleResourceManager::LoadAllAssets()
{
	LoadAssetsRecursively("Assets/");
}

void ModuleResourceManager::LoadAssetsRecursively(std::string dir)
{
	std::vector<std::string>files;
	std::vector<std::string>dirs;

	if (!App->fileSystem->GetDirFiles(dir.c_str(), files, dirs))
	{
		LOG("[error] Trying to acces an invalid directory: %s", dir.c_str());
		return; //if directory doesn't exist return, just for safety
	}

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;
		std::string absPath = dir + str;
		std::string absPathWithoutExt;
		std::string extension;
		App->fileSystem->SeparateExtension(absPath, &extension, &absPathWithoutExt);

		if (extension != ".meta")
		{
			ManageAssetUpdate(absPath.c_str()); //manages the file
		}
		else
		{
			std::string assetMissing = "";
			assetMissing += absPathWithoutExt;
			if (!App->fileSystem->DoesFileExist(assetMissing.c_str()))
			{
				//std::string metapath
				LOG("[warning] Meta file: '%s' is not linked to any asset.\nProceding to delete all unexisting resources...", absPath.c_str());
				//Garbage collector-> check if meta has asset associated, if not delete form lib & memory
				App->fileSystem->DeleteFromAssetsAndLibs(absPath.c_str(), true);
			}

		}
	}

	//for every directory in the current directory, load their assets
	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		const std::string& str = *it;
		std::string newDir = dir + str + "/";
		LoadAssetsRecursively(newDir);
	}
}

void ModuleResourceManager::LoadAssetsFromDir(std::string dir)
{
	std::vector<std::string>files;
	std::vector<std::string>dirs;

	if (!App->fileSystem->GetDirFiles(dir.c_str(), files, dirs))
	{
		return; //if directory doesn't exist return
	}

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;
		std::string extension;
		unsigned int index = str.find_last_of(".");

		if (index < str.size())
		{
			extension = str.substr(index); //look for the last instance of a point. Format should be next
		}

		if (extension != ".meta")
		{
			ImportNewFile(str.c_str());
		}
	}
}

void ModuleResourceManager::LoadResourceIntoMem(Resource* res, bool onlyBase)
{
	ResourceType type = res->GetType();

	char* buffer;
	unsigned int size = App->fileSystem->Load(res->GetLibraryFile().c_str(), &buffer);
	if (size > 0)
	{
		switch (type)
		{
		case ResourceType::TEXTURE:
			Importer::Texture::LoadNewImage(res->GetLibraryFile().c_str(), *res);
			res->SetIsLoadedTo(true);
			break;
		case ResourceType::MESH:
			Importer::Mesh::LoadMesh(buffer, size, *res); //TODO meshes should load general resource
			res->SetIsLoadedTo(true);
			break;
		case ResourceType::SCENE:
			//Scenes are loaded as a combination of Game Object Hiererchies, so they are not a resource per se and so they never count as load in memory
			break;
		case ResourceType::MODEL:
			//Models are loaded as a combination of Game Object Hiererchies, so they are not a resource per se and so they never count as load in memory

			if (onlyBase)
			{
				Importer::Model::LoadModel(res->GetLibraryFile().c_str(), nullptr, true);
			}
			else
			{
				Importer::Model::LoadModel(res->GetLibraryFile().c_str(), App->scene->root);
			}

			break;
		case ResourceType::UNKNOWN:
		default:
			LOG("[error] Trying to load resource with unknown type");
			break;
		}
		RELEASE_ARRAY(buffer);
	}
	else
	{
		LOG("[error] Trying to load a resource that doesn't exist in lib: %s", res->GetLibraryFile().c_str());
	}

}

void ModuleResourceManager::GetAllResourcesOfType(ResourceType rtype, std::vector<Resource*>& ret)
{
	ret.clear();

	std::map<unsigned int, Resource*>::iterator it = resources.begin();

	for (it; it != resources.end(); it++)
	{
		ResourceType type = it->second->GetType();
		if (type == rtype)
		{
			ret.push_back(it->second);
		}
	}
}

Resource* ModuleResourceManager::ManageAssetUpdate(const char* newAssetFile)
{
	Resource* ret = nullptr;

	std::string newPath = App->fileSystem->NormalizePath(newAssetFile);
	App->fileSystem->TransformToRelPath(newPath);
	std::string metaPath = "Assets/" + newPath + ".meta";

	//this line is an experiment TODO
	newPath = "Assets/" + newPath;



	//First, we check if the asset has a .meta associated with it
	if (App->fileSystem->DoesFileExist(metaPath.c_str()))
	{
		//if it exists we check if the file has changed recently

		unsigned long ModNew = App->fileSystem->GetLastModTimeFromPath(newAssetFile);

		char* metaBuffer;
		App->fileSystem->Load(metaPath.c_str(), &metaBuffer);

		Config metaFile(metaBuffer);

		unsigned long ModOld = metaFile.GetNumber("ModDate");
		unsigned int AssID = metaFile.GetNumber("ID");



		if (ModNew != ModOld)//if it has changed re-import, else try load the resource
		{
			LOG("Loading Asset from path: %s", newPath.c_str());
			ret = ReImportExistingFile(newPath.c_str(), AssID);
			//ImportNewFile(newPath.c_str());
		}
		else
		{
			//if asset exists in lib but is not loaded in the database, load it
			std::string found = "";
			FindFileRecursively(std::to_string(AssID), LIB_PATH, found);
			if (found != "")//found in lib?
			{
				std::map<unsigned int, Resource*>::iterator it = resources.find(AssID);
				if (it == resources.end())//if not loaded
				{
					//Add resource into the map
					//aka load it in memory
					ResourceType resType = ResourceTypeFromPath(newAssetFile);
					if (resType != ResourceType::UNKNOWN)
					{
						Resource* res = CreateNewResource(newAssetFile, resType, AssID);
						if (resType == ResourceType::MODEL)
							LoadResourceIntoMem(res, true);
						if (res->IsLoadedInMemory())
						{
							res->UnloadFromMemory();
							LOG("Unloading %i from memory", res->GetLibraryFile());
						}
						ret = res;
					}
				}
				else
				{
					ret = it->second;
				}
			}
			else
			{
				//else if asset doesn't exist in lib regenerate it
				ret = ReImportExistingFile(newPath.c_str(), AssID);
				LOG("Loading Asset from path: %s", newPath.c_str());

			}


		}

	}
	else if (App->fileSystem->DoesFileExist(newPath.c_str()))
	{
		ret = ImportNewFile(newPath.c_str());
		LOG("Loading Asset from path: %s", newPath.c_str());
	}
	else
	{
		LOG("Couldn't find the file with path: %s", newPath.c_str());
	}
	return ret;
}

bool ModuleResourceManager::DeleteItemFromResourcesMap(unsigned int UID)
{
	bool ret = false;
	std::map<unsigned int, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		delete (it->second);
		it->second = nullptr;
		resources.erase(UID);
		ret = true;
	}


	return ret;
}

//Substitute code for this function every time we want to know if a res is in memory TODO
Resource* ModuleResourceManager::FindResInMemory(unsigned int UID)
{
	Resource* ret = nullptr;
	std::map<unsigned int, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		ret = it->second;
	}
	return ret;
}

Resource* ModuleResourceManager::CreateNewResource(const char* assetsFile, ResourceType type, unsigned int existingID)
{
	Resource* ret = nullptr;
	unsigned int newUID = existingID;
	if (newUID == 0)
		newUID = App->renderer3D->seed.Int();

	//TODO create the different types of resource
	switch (type)
	{
	case ResourceType::TEXTURE:
		ret = (Resource*) new ResourceMaterial(newUID);
		break;
	case ResourceType::MESH:
		ret = (Resource*) new ResourceMesh(newUID);
		break;
	case ResourceType::SCENE:
		//TODO
		break;
	case ResourceType::MODEL:
		ret = new Resource(newUID, ResourceType::MODEL); //model is a bundle of resources, we save it a json
		break;
	}

	//adds the resource to the map
	if (ret != nullptr)
	{
		std::map<unsigned int, Resource*>::iterator it = resources.find(ret->GetUID());
		if (it != resources.end())//if already found in mem delete it and regenerate it
		{
			delete it->second;
			it->second = ret;
		}
		else
		{
			resources[newUID] = ret;
		}

		ret->SetAssetPath(assetsFile);
		ret->SetName(GenNameFromPath(*ret));
		ret->SetLibPath(GenLibPath(*ret));
	}

	return ret;
}

std::string ModuleResourceManager::GenLibPath(Resource& res)
{
	ResourceType restType = res.GetType();
	std::string path = "";
	std::string extension = "";

	switch (restType)
	{
	case ResourceType::TEXTURE:
		path = TEXTURE_PATH;
		extension = ".dds";
		break;
	case ResourceType::MESH:
		path = MESH_PATH;
		extension = ".mesh";
		break;
	case ResourceType::SCENE:
		path = SCENE_PATH;
		extension = ".pho";
		break;
	case ResourceType::MODEL:
		path = MODEL_PATH;
		extension = ".model";
		break;
	case ResourceType::UNKNOWN:
	default:
		LOG("[error] Error when Generating lib path for a resource: the resource has unknown type");
		break;
	}

	if (path != "")
	{
		path += std::to_string(res.GetUID());
		path += extension;
	}

	return path;
}

std::string ModuleResourceManager::GenNameFromPath(Resource& res)
{
	std::string path = res.GetAssetFile();
	std::string name = "";
	if (path != "")
	{
		App->fileSystem->SeparatePath(path, nullptr, &name);
		App->fileSystem->SeparateExtension(name, nullptr, &name);
	}


	return name;
}

ResourceType ModuleResourceManager::ResourceTypeFromPath(std::string path)
{
	ResourceType ret = ResourceType::UNKNOWN;
	FileFormats thisFormat = App->fileSystem->CheckFileFormat(path.c_str());

	switch (thisFormat)
	{
	case FileFormats::FBX:
	case FileFormats::OBJ:
		ret = ResourceType::MODEL;
		break;
	case FileFormats::JSON:
		break;

	case FileFormats::PNG:
	case FileFormats::JPG:
	case FileFormats::JPEG:
	case FileFormats::DDS:
	case FileFormats::TGA:
		ret = ResourceType::TEXTURE;
		break;
	case FileFormats::UNDEFINED:
	default:
		LOG("[error]asset from %s has no recognizable format", path.c_str());
		ret = ResourceType::UNKNOWN;
		break;
	}


	return ret;
}


bool ModuleResourceManager::ReleaseSingleResource(unsigned int uid)
{
	bool ret = false;
	//Find if the resource is loaded
	std::map<unsigned int, Resource*>::iterator it = resources.find(uid);

	if (it != resources.end())
	{

		it->second->UnloadFromMemory();
		//release resource here TODO

		ret = true;
	}	return ret;
}

void ModuleResourceManager::SaveResource(Resource& r)
{
	ResourceType type = r.GetType();

	switch (type)
	{
	case ResourceType::TEXTURE:
		Importer::Texture::SaveTexture(r);
		break;
	case ResourceType::SCENE:
		break;
	case ResourceType::MODEL:
		break;
	case ResourceType::UNKNOWN:
		break;
	default:
		break;
	}

	//save pho

}

