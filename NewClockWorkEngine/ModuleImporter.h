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
//
//struct Mesh
//{
//	//Index
//
//	uint* index = nullptr;
//	uint id_index = 0;
//	uint num_index = 0;
//
//	//Normals
//
//	uint	id_normals = 0;
//	uint	num_normals = 0;
//	float* normals = NULL;
//
//	//Colors
//
//	uint	id_colors = 0;
//	uint	num_colors = 0;
//	float* colors = NULL;
//
//	//Coords
//
//	uint	id_texcoords = 0;
//	uint	num_texcoords = 0;
//	float* texcoords = nullptr;
//	uint image_id;
//
//	//Vertex
//
//	uint id_vertex = 0;
//	uint num_vertex = 0;
//	float* vertex = nullptr;
//};

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true);

	~ModuleImporter();

	bool Init();
	bool CleanUp();

	ModuleMesh* UploadFile(const aiScene* scene, aiNode* node, uint id, const char* path);
	GameObject* LoadFBX(const char* path);
	void TextureSetter(const aiScene* scene, aiNode* node, const char* path);
	void RecursiveCall(const aiScene* scene, aiNode* node, aiNode* parentNode, GameObject* parent, const char* path);
	void LoadTexture(const char* path);
	const char* GetMeshFileName();
	const char* GetMaterialFileName();

public:

	const char* meshfilename;
	const char* materialfilename;

public:

	//Mesh myMesh;
	GLuint Gl_Tex;

};
