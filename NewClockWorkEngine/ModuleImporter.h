#pragma once
#include "Globals.h"
#include "Module.h"
#include "ModuleRenderer3D.h"

#include "glew/include/glew.h"
#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

class GameObject;
class ModuleMaterial;
class ModuleMesh;
class aiScene;
struct aiNode;


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

	const char* path;
};


namespace Importer
{
	namespace MeshImporter
	{
		std::vector<Mesh*> Import(const char* file);

		void Save(const Mesh mesh);

		void Load(const char* fileBuffer, Mesh* mesh);

	}
	namespace TextureImp
	{
		Material* Import(const char* path);
		uint CreateTexture(const void* data, uint width, uint height, uint format);
		void InitDevil();
	}
}
