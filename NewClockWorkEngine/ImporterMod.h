#ifndef _IMPORTER_MOD_H_
#define _IMPORTER_MOD_H_

#include "Globals.h"
#include "options.h"
#include <vector>
#include <string>

class Resource;
class ResourceMod;
class ResourceMesh;

struct ModelNode;
class GameObject;
struct GnTexture;
class GnMesh;
class ModuleTransform;
class ModuleMaterial;
class GnJSONArray;
class aiMesh;
class aiScene;
struct aiNode;
class aiMaterial;

#include "MathGeoLib/include/MathGeoLib.h"

namespace ImporterMod
{
	void Import(char* fileBuffer, ResourceMod* resource, uint size);
	void ImportChildren(const aiScene* scene, aiNode* ainode, aiNode* parentAiNode, ModelNode* parentNode, ResourceMod* model);
	void AddParentTransform(ModelNode* node, ModelNode* parentNode);
	void LoadTransform(aiNode* node, ModelNode& modelNode);
	void ReimportFile(char* fileBuffer, ResourceMod* resource, uint size);
	uint64 Save(ResourceMod* model, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceMod* model, uint size);

	GameObject* ConvertToGameObject(ResourceMod* model);
	void ExtractInternalResources(const char* path, std::vector<uint>& meshes, std::vector<uint>& materials);
	void ExtractInternalResources(const char* meta_file, ResourceMod& model);
	bool InternalResourcesExist(const char* path);
	void ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode);
}

#endif

