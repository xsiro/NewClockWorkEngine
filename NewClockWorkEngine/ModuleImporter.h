#pragma once
#include "Globals.h"
#include <vector>
#include "MathGeoLib/src/MathGeoLib.h"

struct aiNode;
struct aiScene;
class ResourceMaterial;
class ResourceMesh;

struct Material
{
	uint width;
	uint height;

	uint id;
	const char* path;
 };

struct Texture
{
	uint width;
	uint height;

	uint id;
	const char* path;
};

struct Mesh
{
	Mesh() {};

	enum Buffers
	{
		index,
		vertex,
		normal,
		texture,
		maxBuffers
	};

	uint buffersId[maxBuffers];
	uint buffersSize[maxBuffers];

	uint* indices = nullptr;
	float* vertices = nullptr;
	float* normals = nullptr;
	float* textureCoords = nullptr;
	AABB aabb;

	const char* path;
};


namespace Importer
{
	namespace MeshImporter
	{
		std::vector<ResourceMesh*>Import(const char* file);
		 
		void LoadNodeMesh(const aiScene* scene, const aiNode* node, std::vector<ResourceMesh*>& meshes);
		uint64 Save(const ResourceMesh mesh);

		void Load(const char* fileBuffer, ResourceMesh* mesh);

	}
	namespace TextureImp
	{
		ResourceMaterial* Import(const char* path);
		uint CreateTexture(const void* data, uint width, uint height, uint format);
		void InitDevil();
	}
	namespace SceneImporter
	{
		void Import(const char* file);

		void ProcessAiNode(const aiScene* scene, const aiNode* node, GameObject* parentObject, const char* file);

		void LoadTransform(const aiNode* node, GameObject* newGameObject);
		void LoadMeshes(const aiScene* scene, const aiNode* node, GameObject* newGameObject);
		void LoadMaterial(const aiScene* scene, const aiNode* node, GameObject* newGameObject, const char* file);
		uint64 Save();	//Store mesh info into custom format file

		void Load(); //Load buffer from custom format file and store into mesh

	}
}
