#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleMaterial.h"
#include "ModuleMesh.h"
#include "ModuleTransform.h"
#include "GameObject.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	materialfilename = "None";
	meshfilename = "None";
}

ModuleImporter::~ModuleImporter()
{

}


bool ModuleImporter::Init() {

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	//UploadFile("Assets/BakerHouse.fbx", App->renderer3D->texture_id);
	LOG("Loading Assimp library");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

bool ModuleImporter::CleanUp()
{
	return true;
}

ModuleMesh* ModuleImporter::UploadFile(const aiScene* scene, aiNode* node, uint id, const char* path) {
	GameObject* parent = nullptr;
	ModuleMesh* myMesh = nullptr;
	ModuleMaterial* material = nullptr;

	aiMesh* currentAiMesh = scene->mMeshes[*node->mMeshes];
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			parent = App->scene_intro->CreateGameObject(nullptr);
			myMesh = (ModuleMesh*)(parent->CreateComponent(ComponentType::Mesh));
			aiMesh* ourMesh = scene->mMeshes[i];
			myMesh->LoadingCheckerTextures();
			// copy vertices
			myMesh->num_vertex = ourMesh->mNumVertices;
			myMesh->vertex = new float[myMesh->num_vertex * 3];
			memcpy(myMesh->vertex, ourMesh->mVertices, sizeof(float) * myMesh->num_vertex * 3);
			LOG("mesh loaded succesfully mesh with %d vertices", myMesh->num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				myMesh->num_index = ourMesh->mNumFaces * 3;
				myMesh->index = new uint[myMesh->num_index];
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3) { LOG("WARNING, geometry face with != 3 indices!"); }

					else { memcpy(&myMesh->index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint)); }
				}
			}

			if (ourMesh->HasNormals())
			{

				myMesh->num_normals = ourMesh->mNumVertices;
				myMesh->normals = new float[myMesh->num_normals * 3];
				for (unsigned int i = 0, v = 0; i < myMesh->num_normals; i++, v += 3)
				{
					myMesh->normals[v] = ourMesh->mNormals[i].x;
					myMesh->normals[v + 1] = ourMesh->mNormals[i].y;
					myMesh->normals[v + 2] = ourMesh->mNormals[i].z;
				}
			}

			if (ourMesh->HasVertexColors(0))
			{
				myMesh->num_colors = ourMesh->mNumVertices;
				myMesh->colors = new float[myMesh->num_colors * 4];
				for (unsigned int i = 0, v = 0; i < myMesh->num_colors; i++, v += 4)
				{
					myMesh->colors[v] = ourMesh->mColors[0][i].r;
					myMesh->colors[v + 1] = ourMesh->mColors[0][i].g;
					myMesh->colors[v + 2] = ourMesh->mColors[0][i].b;
					myMesh->colors[v + 3] = ourMesh->mColors[0][i].a;
				}
			}

			if (ourMesh->HasTextureCoords(0))
			{
				myMesh->num_texcoords = ourMesh->mNumVertices;
				myMesh->texcoords = new float[myMesh->num_texcoords * 2];
				for (unsigned int i = 0, v = 0; i < myMesh->num_texcoords; i++, v += 2)
				{
					myMesh->texcoords[v] = ourMesh->mTextureCoords[0][i].x;
					myMesh->texcoords[v + 1] = ourMesh->mTextureCoords[0][i].y;
				}
				myMesh->image_id = id;
			}
			myMesh->LoadFBXBuffer();
			LOG("%i", myMesh->num_index);
		}

	}

	else
		LOG("Error loading mesh % s");

	return myMesh;
}

void ModuleImporter::LoadTexture(const char* path)
{
	ILuint Il_Tex;
	materialfilename = path;
	ilGenImages(1, &Il_Tex);
	ilBindImage(Il_Tex);
	ilLoadImage(path);
	Gl_Tex = ilutGLBindTexImage();
	ilDeleteImages(1, &Il_Tex);
	if (Il_Tex != NULL)
	{
		LOG("Successfuly loaded %s texture", path);
	}
	else {
		LOG("Error loading the texture!");
	}
}
const char* ModuleImporter::GetMeshFileName() {

	return meshfilename;

}

const char* ModuleImporter::GetMaterialFileName() {

	return materialfilename;

}

GameObject* ModuleImporter::LoadFBX(const char* path)
{

	GameObject* parent = new GameObject();
	meshfilename = path;
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* parentNode = scene->mRootNode;
		RecursiveCall(scene, parentNode, nullptr, parent, path);
		aiReleaseImport(scene);
	}

	return parent;
}

void ModuleImporter::RecursiveCall(const aiScene* scene, aiNode* node, aiNode* parentNode, GameObject* parent, const char* path)
{
	GameObject* gameObject = new GameObject();

	if (node->mMeshes != nullptr)
	{
		ModuleMesh* mesh = App->importer->UploadFile(scene, node, true, path);
		gameObject->components.push_back(mesh);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		RecursiveCall(scene, node->mChildren[i], node, gameObject, path);
	}
	parent->children.push_back(gameObject);
	gameObject->parent = parent;
}