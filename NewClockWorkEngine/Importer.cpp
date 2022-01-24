#include "Importer.h"
#include "Globals.h"
#include "Application.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

//Saving things in own format
#include "PhysFS/include/physfs.h"

//devil initialization
#include "DevIL/include/IL/il.h"  //loading-saving-converting iamges
#include "DevIL/include/IL/ilu.h" //middle level library for image manipulation
#include "DevIL/include/IL/ilut.h" //high level library for displaying images

//note that those 3 devil libraries are hierarchical and so to include one of them we have to include the ones before

#pragma comment(lib,"DevIL/libx86/DevIL.lib")
#pragma comment(lib,"DevIL/libx86/ILU.lib")
#pragma comment(lib,"DevIL/libx86/ILUT.lib")

#include<deque>
//#include"glmath.h"
#include"MathGeoLib/include/MathGeoLib.h"

#include "GameObject.h"
#include "Component.h"
#include "ModuleMesh.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ModuleMaterial.h"
#include "ModuleTransform.h"
#include "ModuleCamera.h"
#include "AudioSource.h"
#include "ModuleControl.h"
#include "ModuleReverbZone.h"
#include "Config.h"
#include "ModuleResourceManager.h"
#include "Color.h"
#include <map>

bool Importer::InitializeDevIL()
{
	//initialize IL
	ilInit();
	//initialize ILU
	iluInit();
	//initialize ILUT with OpenGl Support
	ilutInit();
	ilutRenderer(ILUT_OPENGL); //call this before using any ilut function

	return true;
}

bool Importer::Texture::ImportImage(const char* Buffer, unsigned int Length, Resource& textureToFill)
{
	ResourceMaterial* t = (ResourceMaterial*)&textureToFill;

	ILuint newImage = 0;
	ilGenImages(1, &newImage);
	ilBindImage(newImage);

	bool ret = ilLoadL(IL_TYPE_UNKNOWN, Buffer, Length);


	if (!ret)
	{
		ILenum error;
		error = ilGetError();
		LOG("\n[error]Could not load an miage from buffer");
		//LOG("[error] %d :\n %s", error, iluErrorString(error));
		ilDeleteImages(1, &newImage);
	}
	else if (ret = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
	{
		ilBindImage(newImage);
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		t->GenTextureFromName(newImage);
		//ilDeleteImages(1, &newImage);
	}
	return ret;
}

bool Importer::Texture::LoadNewImage(const char* libPath, Resource& textureToFill)
{
	char* buffer;
	unsigned int size = App->fileSystem->Load(libPath, &buffer);
	ResourceMaterial* t = (ResourceMaterial*)&textureToFill;

	ILuint newImage = 0;
	ilGenImages(1, &newImage);
	ilBindImage(newImage);

	bool ret = ilLoadL(IL_TYPE_UNKNOWN, buffer, size);


	if (!ret)
	{
		ILenum error;
		error = ilGetError();
		LOG("\n[error]Could not load an miage from buffer");
		//LOG("[error] %d :\n %s", error, iluErrorString(error));
		ilDeleteImages(1, &newImage);
	}
	else if (ret = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
	{

		t->GenTextureFromName(newImage);
		t->ilImageID = -1;
		ilDeleteImages(1, &newImage);
		RELEASE_ARRAY(buffer);
	}
	return ret;
}



bool Importer::Model::ImportModel(const char* Buffer, unsigned int Length, const char* relativePath, Resource* res)
{
	bool ret = false;

	if (Length > 0)
	{

		const aiScene* scene = aiImportFileFromMemory(Buffer, Length, aiProcessPreset_TargetRealtime_MaxQuality, nullptr); //nullptr as we need no external libs to hepl import

											//aiImportFileFromMemory										
		LOG("Importing 3D asset from buffer");

		if (scene != nullptr)
		{
			std::string pathWithoutFile;
			std::string filename;
			App->fileSystem->SeparatePath(relativePath, &pathWithoutFile, &filename);
			// Use scene->mNumMeshes to iterate on scene->mMeshes array

			std::vector<ResourceMesh*> meshesToAssign;
			//load each mesh here
			for (int i = 0; i < scene->mNumMeshes; i++)
			{
				//scene->mMeshes[i]->mName; TODO GET MESH NAME TO ASSIGN IT TO THE RESOURCE
				std::string meshName = scene->mMeshes[i]->mName.C_Str();
				if (meshName == "")
					meshName = "Untitled Mesh";

				meshName = filename + "(" + std::to_string(i) + ")/" + meshName;

				unsigned int newUID = res->GetUID() + 1 + i;
				ResourceMesh* currMesh = (ResourceMesh*)App->rManager->CreateNewResource(relativePath, ResourceType::MESH, newUID);
				currMesh->SetName(meshName);
				Mesh::ImportRMesh(scene->mMeshes[i], *currMesh); //Take the mesh out of the fbx in assets and plop it into engine
				char* auxB;
				if (Mesh::SaveMesh(*currMesh, &auxB) > 0) //Here we save to lib the mesh portion of our model (from engine to lib)
				{
					RELEASE_ARRAY(auxB);
				}
				if (currMesh != nullptr)
					meshesToAssign.push_back(currMesh);

			}

			std::map<unsigned int, ResourceMaterial*> texturesToAssign;
			//load every material here
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				aiMaterial* material = scene->mMaterials[i];
				aiString path;
				unsigned int numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
				if (numTextures > 0)
				{
					material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

					std::string textureName = App->fileSystem->NormalizePath(path.C_Str());
					App->fileSystem->SeparatePath(textureName, nullptr, &textureName);
					path = pathWithoutFile + textureName.c_str();//this is the same path as the asset

					Resource* r = App->rManager->ManageAssetUpdate(path.C_Str());//Here we import the non-mesh portion of our model: textures!
					if (r == nullptr)
					{
						//search for the path on assets
						std::string fullPath = "";
						App->fileSystem->SeparateExtension(textureName, nullptr, &textureName);
						App->fileSystem->FindFileInDirectory(textureName, "Assets/", fullPath);//try to find texture in assets

						r = App->rManager->ManageAssetUpdate(fullPath.c_str());
					}

					if (r != nullptr)
					{
						texturesToAssign[i] = (ResourceMaterial*)r;
					}
				}
			}

			aiNode* node = scene->mRootNode;

			std::deque<aiNode*>parents;
			std::deque<GameObject*>gameObjParents;

			parents.push_back(node);

			std::string name = "Untitled";
			std::string fbxName;

			App->fileSystem->SeparatePath(relativePath, nullptr, &fbxName);
			int dotIndex = fbxName.find_last_of(".");
			if (dotIndex >= 0 && dotIndex < fbxName.length())
			{
				name = fbxName.substr(0, dotIndex);
				name += " File";
			}
			else if (node->mName.C_Str() != "")
			{
				name = "Default ";
				name += node->mName.C_Str();
			}

			GameObject* pObj = new GameObject(App->scene->root, name, float4x4::identity);
			GameObject* root = pObj;
			gameObjParents.push_back(pObj);//first node is root and doesn't have mesh nor material

			while (parents.size() > 0)
			{
				std::deque<aiNode*> parentsCopy = parents;

				for (int i = 0; i < parentsCopy.size(); i++)
				{
					aiNode* currentParent = parents[0];
					parents.pop_front();

					GameObject* currObjParent = gameObjParents[0];
					gameObjParents.pop_front();


					for (int j = 0; j < currentParent->mNumChildren; j++)
					{
						//ResourceMesh* auxMesh = nullptr;
						parents.push_back(currentParent->mChildren[j]);
						//aiMesh* newMesh = nullptr;
						//if (parents.back()->mNumMeshes > 0) {
							//newMesh = scene->mMeshes[parents.back()->mMeshes[0]];//loads a mesh from index
							//create game object and save it into gameObjParents (its parent is currObjParent)
							//auxMesh = (ResourceMesh*)App->rManager->CreateNewResource(relativePath, ResourceType::MESH);
							//Mesh::ImportRMesh(newMesh, *auxMesh); //Take the mesh out of the fbx in assets and plop it into engine
							//char* auxB = "y";
							//Mesh::SaveMesh(*auxMesh, &auxB); //Here we save to lib the mesh portion of our model (from engine to lib)
							//LOG("debug");
						//}
						//gameObjParents.push_back(LoadGameObjFromAiMesh(auxMesh, newMesh, scene, parents.back(), currObjParent, pathWithoutFile)); //Here we import tex!


						//here we create the game objects (children) from their node (parents.back()) and a parent (currObjParent)
						//assigns game object name
						std::string name = "Untitled";
						if (parents.back()->mName.C_Str() != "")
						{
							name = parents.back()->mName.C_Str();
						}

						//assigns game object parent
						GameObject* newParent = currObjParent;
						if (currObjParent == nullptr)
							newParent = App->scene->root;

						//transform calculations
						aiVector3D translation, scaling;
						aiQuaternion rotation;
						parents.back()->mTransformation.Decompose(scaling, rotation, translation);

						float3 newTranslation = float3(translation.x, translation.y, translation.z);

						Quat newRot = Quat(rotation.x, rotation.y, rotation.z, rotation.w);

						float4x4 newTransform = float4x4::FromTRS(newTranslation, newRot.ToFloat4x4(), float3(scaling.x, scaling.y, scaling.z));



						//creates new game object
						GameObject* newObj = new GameObject(newParent, name, newTransform);
						ResourceMesh* myMesh = nullptr;
						ResourceMaterial* myTexture = nullptr;
						if (parents.back()->mNumMeshes > 0)
						{
							myMesh = meshesToAssign[parents.back()->mMeshes[0]];
						}

						if (myMesh != nullptr)//assign mesh
						{
							ModuleMesh* com = (ModuleMesh*)newObj->CreateComponent(ComponentType::MESH);
							com->SetNewResource(myMesh->GetUID());


							int matIndex = scene->mMeshes[parents.back()->mMeshes[0]]->mMaterialIndex;
							aiMaterial* aiMat = scene->mMaterials[matIndex];
							std::map<unsigned int, ResourceMaterial*>::iterator it = texturesToAssign.find(matIndex);
							aiColor4D col;
							bool hasMat = (aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &col) == aiReturn_SUCCESS);
							if (it != texturesToAssign.end() || hasMat)
							{
								ModuleMaterial* com = (ModuleMaterial*)newObj->CreateComponent(ComponentType::MATERIAL);
								if (it != texturesToAssign.end() && it->second != nullptr)
								{
									com->SetNewResource(it->second->GetUID());
								}
								if (hasMat)
								{
									com->matCol.r = col.r;
									com->matCol.g = col.g;
									com->matCol.b = col.b;
									com->matCol.a = col.a;

								}
							}

						}

						gameObjParents.push_back(newObj);

					}



				}
			}
			Model::SaveModel(root, res);//take the model resource and plop it into lib. It has info such as ID, GO list, Components array...
			delete root;
			aiReleaseImport(scene);
			texturesToAssign.clear();
			meshesToAssign.clear();
		}
		else
		{
			LOG("[error]Error loading scene %s", Buffer);
			ret = false;
		}


	}
	else
	{
		LOG("[error]Error scene with path '%s' doesnt exist", relativePath);
	}
	return ret;
}

//TODO IMPORTANT take model name as a parameter here and assign that name to their childs (every resource should have a name)
bool Importer::Model::LoadModel(const char* libPath, GameObject* root, bool onlyBase)
{
	GameObject* newRoot = nullptr;
	if (!onlyBase)
	{
		newRoot = root;
	}
	else//if we are only going to load the objects for the preview of the resources, we will create a root GO and delete it before exiting the method
	{
		newRoot = new GameObject(nullptr, "", float4x4::identity);
	}
	bool ret = false;
	char* buffer = "";
	App->fileSystem->Load(libPath, &buffer);
	Config file(buffer);

	
	std::map<int, GameObject*> createdGameObjects;
	Config_Array gameObjects_array = file.GetArray("GameObjects");
	for (uint i = 0; i < gameObjects_array.GetSize(); ++i)
	{
		//Pinpoint the GO 
		Config gameObject_node = gameObjects_array.GetNode(i);

		//Get its Global mat needed to call constructor
		float4x4 auxGlobalMat = gameObject_node.GetArray("Transform").GetMatTransform(0); //they shouldnt have more than 1 Global transform...

		//Parent setup
		GameObject* parent = nullptr;
		std::map<int, GameObject*>::iterator it = createdGameObjects.find(gameObject_node.GetNumber("ParentUID"));
		if (it != createdGameObjects.end())
			parent = it->second;

		//Create the GO
		GameObject* gameObject = new GameObject(parent ? parent : newRoot, gameObject_node.GetString("Name").c_str(), auxGlobalMat);
		ret = true;


		//Set properties of GO
		gameObject->ID = gameObject_node.GetNumber("ID");
		createdGameObjects[gameObject->ID] = gameObject;
		gameObject->isActive = gameObject_node.GetBool("Active");
		gameObject->focused = gameObject_node.GetBool("Focused");
		gameObject->selected = gameObject_node.GetBool("Selected");
		//get the components
		Config_Array components = gameObject_node.GetArray("Components");

		for (uint i = 0; i < components.GetSize(); i++)
		{
			Config comp = components.GetNode(i);
			ComponentType type = (ComponentType)((int)comp.GetNumber("ComponentType"));
			if (Component* component = gameObject->CreateComponent(type, comp.GetNumber("ID")))
			{
				//Resource* newR = App->rManager->RequestNewResource(comp.GetNumber("ResourceID"));
				unsigned int newUID = comp.GetNumber("ResourceID");
				std::string resourceName = comp.GetString("ResourceName");
				Resource* r = nullptr;
				switch (type)
				{
				case ComponentType::CAMERA:
					//Hold your horses
					break;
				case ComponentType::MESH:
				{
					//check if the component has a resource (resourceid!=0)
					if (newUID != 0)
					{

						r = App->rManager->FindResInMemory(newUID);
						if (r == nullptr)//if not found in memory find it in lib
						{
							r = App->rManager->CreateNewResource("UntitledForNow", ResourceType::MESH, newUID);//TODO asset path should be FBX asset path
							r->SetName(resourceName);
							App->rManager->LoadResourceIntoMem(r);
						}

						component->SetNewResource(newUID);
					}

				}
				break;
				case ComponentType::MATERIAL:
				{
					//check if the component has a resource here (resourceid!=0)
					if (newUID != 0)
					{

						r = App->rManager->FindResInMemory(newUID);
						if (r == nullptr)//if not found in memory find it in lib
						{
							std::string fullpath = "";

							App->fileSystem->FindFileInDirectory(std::to_string(newUID), TEXTURE_PATH, fullpath);

							if (fullpath != "")
							{
								r = App->rManager->CreateNewResource("UntitledForNow", ResourceType::TEXTURE, newUID);//TODO asset path should be texture asset path
								r->SetName(resourceName);
								App->rManager->LoadResourceIntoMem(r);

							}
						}
						component->SetNewResource(newUID);
					}

					ModuleMaterial* m = (ModuleMaterial*)component;
					m->matCol.r = comp.GetNumber("Color R");
					m->matCol.g = comp.GetNumber("Color G");
					m->matCol.b = comp.GetNumber("Color B");
					m->matCol.a = comp.GetNumber("Color A");
				}
				break;
				case ComponentType::TRANSFORM:
					//Nothing: this is already done in constructor
					break;
				default:
					LOG("[error] Tried to load a model, but the material with ID %i of Game Object %s had an unexpected type", component->ID, component->owner->GetName());
					break;
				}

				
			}
		}
	
	}
	if (onlyBase)
	{
		delete newRoot;
		newRoot = nullptr;
	}
	return ret;
}

unsigned int Importer::Model::SaveModel(GameObject* root, Resource* ret)
{
	char* buffer = ""; 
	Config file;
	Config_Array ArrayGameObjects = file.SetArray("GameObjects");
	std::vector<GameObject*> gameObjects;
	SeekChildrenRecurvisely(root, gameObjects);
	//We want so save root!!

	for (unsigned int i = 0; i < gameObjects.size(); ++i)
	{
		SerializeGameObject(ArrayGameObjects.AddNode(), gameObjects[i]);
	}

	unsigned int size = file.Serialize(&buffer);

	std::string name = ret->GetLibraryFile();

	App->fileSystem->SavePHO(name.c_str(), buffer, size);


	return size;
}

//returns 0 by default if something failed
unsigned int Importer::LoadPureImageGL(const char* path)
{
	unsigned int ID = 0;
	char* new_buffer;
	unsigned int length = App->fileSystem->Load(path, &new_buffer);

	ilGenImages(1, &ID);
	ilBindImage(ID);

	bool ret = ilLoadL(IL_TYPE_UNKNOWN, new_buffer, length);

	if (!ret)
	{
		ILenum error;
		error = ilGetError();
		LOG("\n[error]Could not load an image from buffer");
		//LOG("[error] %d :\n %s", error, iluErrorString(error));
		if (length != 0)
			RELEASE_ARRAY(new_buffer);
		ilDeleteImages(1, &ID);
	}
	else if (ret = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
	{
		ilBindImage(ID);

		//get properties
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}
		//gen texture
		unsigned int openGlId;
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenTextures(1, &openGlId);
		glBindTexture(GL_TEXTURE_2D, openGlId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		glBindTexture(GL_TEXTURE_2D, 0);

		ilBindImage(0);

		ilDeleteImages(1, &ID);
		RELEASE_ARRAY(new_buffer);
		return openGlId;
	}
	return 0;
}

unsigned int Importer::Mesh::SaveMesh(Resource& meshA, char** buffer)
{
	ResourceMesh* mesh = (ResourceMesh*)&meshA;

	unsigned int ranges[5] = { mesh->indices.size(), mesh->vertices.size(), mesh->normals.size(),mesh->smoothedNormals.size(), mesh->texCoords.size() };
	unsigned int  size =
		sizeof(ranges)
		+ sizeof(uint) * mesh->indices.size()
		+ sizeof(float) * mesh->vertices.size()
		+ sizeof(float) * mesh->normals.size()
		+ sizeof(float) * mesh->smoothedNormals.size()
		+ sizeof(float) * mesh->texCoords.size();

	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;

	unsigned int bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(unsigned int) * mesh->indices.size();
	memcpy(cursor, &mesh->indices[0], bytes);
	cursor += bytes;

	// Store vertex
	bytes = sizeof(float) * mesh->vertices.size();
	memcpy(cursor, &mesh->vertices[0], bytes);
	cursor += bytes;

	// Store normals
	bytes = sizeof(float) * mesh->normals.size();
	memcpy(cursor, &mesh->normals[0], bytes);
	cursor += bytes;

	// Store smooth normals
	bytes = sizeof(float) * mesh->smoothedNormals.size();
	memcpy(cursor, &mesh->smoothedNormals[0], bytes);
	cursor += bytes;

	// Store tex coords
	bytes = sizeof(float) * mesh->texCoords.size();
	memcpy(cursor, &mesh->texCoords[0], bytes);
	cursor += bytes;

	App->fileSystem->SavePHO(meshA.GetLibraryFile().c_str(), fileBuffer, size);
	*buffer = fileBuffer;
	LOG(*buffer);
	return size;
}

unsigned int Importer::Texture::SaveTexture(Resource& texture)
{
	ResourceMaterial* t = (ResourceMaterial*)&texture;
	if (t->ilImageID != -1)
	{
		ILuint image = t->ilImageID;
		ILubyte* data = nullptr;
		ilBindImage(image);

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		unsigned int size = ilSaveL(IL_DDS, nullptr, 0);//get size

		if (size > 0)
		{
			data = new ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) <= 0)
			{
				return 0;
			}

		}
		else
		{
			LOG("[error] saving the texture, size is 0");
		}

		//unsigned int values[1] = { t->GetAssetFile().length() };
		//std::string auxPath = t->GetAssetFile();


		//unsigned int size = sizeof(values) + auxPath.length();
		//char* fileBuffer = new char[size]; // Allocate
		//char* cursor = fileBuffer;

		//// First store values
		//unsigned int bytes = sizeof(values);
		//memcpy(cursor, values, bytes);
		//cursor += bytes;

		//// Store path
		//bytes = auxPath.length();
		//memcpy(cursor, auxPath.c_str(), bytes);
		//cursor += bytes;

		if (data != nullptr)
		{
			App->fileSystem->SavePHO(t->GetLibraryFile().c_str(), data, size);
			RELEASE_ARRAY(data);
		}
		ilDeleteImages(1, (unsigned int*)&t->ilImageID);
		t->ilImageID = -1;

		return size;
	}
	LOG("[error] saving the texture, this texture IL ID doesn't exist");

	return 0;
}

unsigned int Importer::Camera::SaveCamera(ModuleCamera* aux, char* buffer)
{
	//float nearPlaneDist;
	//float farPlaneDist;
	//float FoV;
	float values[4] = { aux->GetNearPlaneDist(), aux->GetFarPlaneDist(), aux->GetFoV(),aux->GetAspectRatio() };

	unsigned int size = sizeof(values);
	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;

	// First store values
	unsigned int bytes = sizeof(values);
	memcpy(cursor, values, bytes);
	cursor += bytes;

	std::string fileName = SCENE_PATH;
	fileName += "testingCamera.pho";
	App->fileSystem->SavePHO(fileName.c_str(), fileBuffer, size);
	buffer = fileBuffer;
	return size;
}

void Importer::Camera::SaveComponentCamera(Config& config, Component* cam)
{
	ModuleCamera* camera = (ModuleCamera*)cam;

	config.SetNumber("FOV", camera->GetFoV()); //this is the x, y is calculated afterwards
	config.SetNumber("NearPlane", camera->GetNearPlaneDist());
	config.SetNumber("FarPlane", camera->GetFarPlaneDist());
	config.SetNumber("AspectRatio", camera->GetAspectRatio());
	config.SetBool("IsCulling", camera->GetIsCulling());
	Color c = camera->GetBackgroundCol();
	config.SetNumber("BG R", c.r);
	config.SetNumber("BG G", c.g);
	config.SetNumber("BG B", c.b);
	config.SetNumber("BG A", c.a);
	//config.SetBool("MainCamera",camera->main);
}

void Importer::Texture::SaveComponentMaterial(Config& config, Component* auxMat)
{
	ModuleMaterial* mat = (ModuleMaterial*)auxMat;
	Color c = mat->matCol;

	config.SetNumber("Color R", c.r);
	config.SetNumber("Color G", c.g);
	config.SetNumber("Color B", c.b);
	config.SetNumber("Color A", c.a);
}

bool Importer::Mesh::LoadMesh(char* buffer, unsigned int Length, Resource& meshToFillA)
{
	ResourceMesh* meshToFill = (ResourceMesh*)&meshToFillA;
	std::vector<float> vertices; std::vector<unsigned int> indices; std::vector<float> normals; std::vector<float> smoothedNormals; std::vector<float> texCoords;
	char* cursor = buffer; //where in memory does the file start (pointer to first memory access)

	// amount of indices / vertices / smoothed vertices  / normals / texture_coords
	uint ranges[5]; //necessarily hardcoded
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;
	int num_indices = ranges[0];
	int num_vertices = ranges[1];
	int num_normals = ranges[2];
	int num_smoothedNormals = ranges[3];
	int num_tex = ranges[4];

	// Load indices
	bytes = sizeof(uint) * num_indices;
	indices.resize(num_indices); //THIS IS THE LESS GHETTO WAY TO DO IT

	memcpy(&indices[0], cursor, bytes); //&indices[0] since we only need to point where he needs to start writing. bytes will tell it when to stop
	cursor += bytes;


	// Load vertex
	bytes = sizeof(float) * num_vertices;
	vertices.resize(num_vertices);

	memcpy(&vertices[0], cursor, bytes);
	cursor += bytes;


	// Load normals
	bytes = sizeof(float) * num_normals;
	normals.resize(num_normals);

	memcpy(&normals[0], cursor, bytes);
	cursor += bytes;

	// Load smoothed normals
	bytes = sizeof(float) * num_smoothedNormals;
	smoothedNormals.resize(num_smoothedNormals);

	memcpy(&smoothedNormals[0], cursor, bytes);
	cursor += bytes;


	// Load texcoords
	bytes = sizeof(float) * num_tex;
	texCoords.resize(num_tex);

	memcpy(&texCoords[0], cursor, bytes);
	cursor += bytes;


	//Remake the mesh

	meshToFill->vertices = vertices;
	meshToFill->indices = indices;
	meshToFill->normals = normals;
	meshToFill->texCoords = texCoords;
	meshToFill->smoothedNormals = smoothedNormals;
	meshToFill->GenerateBuffers();

	if (!meshToFill->vertices.empty() && !meshToFill->indices.empty()) { return true; }
	else {
		LOG("Malformed mesh loaded from PHO");
		return false;
	};

}


bool Importer::Camera::LoadCameraFromPho(char* buffer, unsigned int Lenght)
{
	//float nearPlaneDist; float farPlaneDist; float FoV; float aspectRatio;

	char* cursor = buffer; //where in memory does the file start (pointer to first memory access)

	// values stored
	float values[4]; //necessarily hardcoded
	unsigned int bytes = sizeof(values);
	memcpy(values, cursor, bytes);
	cursor += bytes;

	// Load global mat (float4x4);
	float nearPlaneDist = values[0];
	float farPlaneDist = values[1];
	float FoV = values[2];
	float aspectRatio = values[3];

	return true;
}

void Importer::SerializeGameObject(Config& config, GameObject* gameObject) //serialize GO
{
	config.SetNumber("ID", gameObject->ID);
	config.SetNumber("ParentUID", gameObject->parent ? gameObject->parent->ID : 0);
	config.SetString("Name", gameObject->GetName().c_str());
	config.SetBool("Active", gameObject->isActive);
	config.SetBool("Focused", gameObject->focused);
	config.SetBool("Selected", gameObject->selected);

	//Global transform
	const ModuleTransform* transform = gameObject->GetComponent<ModuleTransform>();
	config.SetArray("Transform").Add4x4Mat(transform->GetGlobalTransform()); //yeah, we save it raw. Deal with it


	Config_Array compConfig = config.SetArray("Components");
	std::vector<Component*> components = gameObject->GetAllComponents();

	for (unsigned int i = 0; i < components.size(); i++)
	{
		SaveComponentRaw(compConfig.AddNode(), components[i]);
	}
}

unsigned int Importer::SerializeScene(GameObject* root, char** TrueBuffer)	 //serialize scene
{
	/*
	1: search for children
	2. save gameobject -> UID,UID of parent (0 if root, ghetto Alex), save components as children
	3. save its components -> type, active, switch to know that extra info to save
	4. recurr
	*/

	Config file;
	Config_Array ArrayGameObjects = file.SetArray("GameObjects");
	std::vector<GameObject*> gameObjects;
	SeekChildrenRecurvisely(root, gameObjects);
	//We don't want to save root
	gameObjects.erase(gameObjects.begin());
	for (unsigned int i = 0; i < gameObjects.size(); ++i)
	{
		SerializeGameObject(ArrayGameObjects.AddNode(), gameObjects[i]);
	}

	unsigned int size = file.Serialize(TrueBuffer);
	return size;
}

void Importer::LoadScene(char* buffer, GameObject* sceneRoot)
{
	//App->renderer3D->SetNewCullingCam(nullptr);
	if (App->renderer3D->activeCam != nullptr)
	{
		App->renderer3D->activeCam->SetAsCullingCam(false);
	}

	App->audioManager->UnRegisterAllAudioObjs();

	if (!App->scene->selectedGameObjs.empty())
	{
		App->scene->SetSelectedGameObject(nullptr);
	}

	//Open the bufer you are going to be reading
	Config file(buffer);
	//set the root of the new scene
	//App->editor3d->root = sceneRoot;

	std::map<int, GameObject*> createdGameObjects;
	Config_Array gameObjects_array = file.GetArray("GameObjects");
	GameObject* focusedGameObject = nullptr;
	for (uint i = 0; i < gameObjects_array.GetSize(); ++i)
	{
		//Pinpoint the GO 
		Config gameObject_node = gameObjects_array.GetNode(i);

		//Get its Global mat needed to call constructor
		float4x4 auxGlobalMat = gameObject_node.GetArray("Transform").GetMatTransform(0); //they shouldnt have more than 1 Global transform...

		//Parent setup
		GameObject* parent = nullptr;
		std::map<int, GameObject*>::iterator it = createdGameObjects.find(gameObject_node.GetNumber("ParentUID"));
		if (it != createdGameObjects.end())
			parent = it->second;
		//Create the GO


		GameObject* gameObject = new GameObject(parent ? parent : sceneRoot, gameObject_node.GetString("Name").c_str(), auxGlobalMat, true, false);

		//Set properties of GO
		gameObject->ID = gameObject_node.GetNumber("ID");
		createdGameObjects[gameObject->ID] = gameObject;
		gameObject->isActive = gameObject_node.GetBool("Active");
		gameObject->focused = gameObject_node.GetBool("Focused");
		gameObject->selected = gameObject_node.GetBool("Selected");

		//TODO for the future here we will load every selected obj and we will store the focused one to add it at the end of the load
		if (gameObject->selected)
		{
			App->scene->SetSelectedGameObject(gameObject, true);
			if (gameObject->focused)
			{
				focusedGameObject = gameObject;
			}
		}

		//get the components
		Config_Array components = gameObject_node.GetArray("Components");

		for (uint i = 0; i < components.GetSize(); i++)
		{
			Config comp = components.GetNode(i);
			ComponentType type = (ComponentType)((int)comp.GetNumber("ComponentType"));

			if (Component* component = gameObject->CreateComponent(type, comp.GetNumber("ID")))
			{
				//component->ID = comp.GetNumber("ID");
				component->SetActive(comp.GetBool("Active"));

				unsigned int newUID = comp.GetNumber("ResourceID");
				std::string resourceName = comp.GetString("ResourceName");

				Resource* r = nullptr;

				switch (type)
				{
				case ComponentType::CAMERA:
				{
					//Hold your horses, if you put brackets you can create variables here
					ModuleCamera* cam = (ModuleCamera*)component;
					cam->SetNearPlane(comp.GetNumber("NearPlane"));
					cam->SetFarPlane(comp.GetNumber("FarPlane"));
					cam->SetNewAspectRatio(comp.GetNumber("AspectRatio"));
					cam->SetNewFoV(comp.GetNumber("FOV"));
					bool isCulling = comp.GetBool("IsCulling");
					if (isCulling)cam->SetAsCullingCam(true);

					Color c;
					c.r = comp.GetNumber("BG R");
					c.g = comp.GetNumber("BG G");
					c.b = comp.GetNumber("BG B");
					c.a = comp.GetNumber("BG A");
					cam->SetBackgroundCol(c);

				}
				break;
				case ComponentType::MESH:
					if (newUID != 0)
					{
						r = App->rManager->FindResInMemory(newUID);
						if (r == nullptr)//if not found in memory find it in lib
						{
							r = App->rManager->CreateNewResource("UntitledForNow", ResourceType::MESH, newUID);//TODO asset path should be FBX asset path
							r->SetName(resourceName);
							App->rManager->LoadResourceIntoMem(r);
						}

						//component.chutame_la_mesh
						component->SetNewResource(newUID);
					}

					break;
				case ComponentType::MATERIAL:
				{

					if (newUID != 0)
					{
						r = App->rManager->FindResInMemory(newUID);
						if (r == nullptr)//if not found in memory find it in lib
						{
							r = App->rManager->CreateNewResource("UntitledForNow", ResourceType::TEXTURE, newUID);//TODO asset path should be texture asset path
							r->SetName(resourceName);
							App->rManager->LoadResourceIntoMem(r);
						}
						//component.chutame_la_tex
						component->SetNewResource(newUID);
					}
					ModuleMaterial* m = (ModuleMaterial*)component;
					m->matCol.r = comp.GetNumber("Color R");
					m->matCol.g = comp.GetNumber("Color G");
					m->matCol.b = comp.GetNumber("Color B");
					m->matCol.a = comp.GetNumber("Color A");
				}
				break;
				case ComponentType::AUDIO_LISTENER:
				{
					//TODO load audio listener in here
					AudioListener* listener = (AudioListener*)component;
					bool isListener = comp.GetBool("IsListener");
					if (isListener)listener->SetAsListener(true);
				}
				break;
				case ComponentType::AUDIO_SOURCE:
				{
					AudioSource* source = (AudioSource*)component;
					//TODO load audio source in here
					source->SetVolume(comp.GetNumber("Volume"));
					source->SetSecondsToChangeMusic(comp.GetNumber("SecondsToNextMusic"));
					source->SetUserPitch(comp.GetNumber("UserPitch"));
					Config_Array confArray = comp.GetArray("Events");
					for (int j = 0; j < confArray.GetSize(); j++)
					{
						Config& eventConf = confArray.GetNode(j);
						std::string evName = eventConf.GetString("Name");
						AudioEventType evType = (AudioEventType)(int)eventConf.GetNumber("Type");
						source->CreateAudioEvent(evName, evType);
					}
				}
				break;
				case ComponentType::REVERB_ZONE:
				{
					ModuleReverbZone* reverb = (ModuleReverbZone*)component;
					reverb->targetBus = comp.GetString("TargetBus");
					reverb->revValue = comp.GetNumber("RevValue");
					reverb->SetReverbZone(float3(
						comp.GetNumber("DimensionsX"),
						comp.GetNumber("DimensionsY"),
						comp.GetNumber("DimensionsZ")));
				}
				break;
				case ComponentType::TRANSFORM:
					//Nothing: this is already done in constructor
					break;
				case ComponentType::CONTROL:
				{

					ModuleControl* control = (ModuleControl*)component;
					control->SetSpeed(comp.GetNumber("Speed"));
					break;
				}
				default:
					LOG("[error] Tried to load a model, but the material with ID %i of Game Object %s had an unexpected type", component->ID, component->owner->GetName());
					break;
				}


			


			}

		}
	
	}

	App->scene->SetSelectedGameObject(focusedGameObject, true);

}

void Importer::SeekChildrenRecurvisely(GameObject* root, std::vector<GameObject*>& vectorToFill)
{
	vectorToFill.push_back(root);

	for (unsigned int i = 0; i < root->children.size(); i++)
	{
		SeekChildrenRecurvisely(root->children[i], vectorToFill);
	}
}

void Importer::SaveComponentRaw(Config& config, Component* component)
{
	config.SetNumber("ComponentType", (int)component->GetType());
	config.SetNumber("ID", (int)component->ID);
	config.SetBool("Active", component->IsActive());
	unsigned int resID = component->GetResourceID();
	config.SetNumber("ResourceID", resID);

	std::string resName = "";
	Resource* currResource = App->rManager->FindResInMemory(resID);
	if (currResource)
	{
		resName = currResource->GetName();
	}
	config.SetString("ResourceName", resName.c_str());


	switch (component->GetType())
	{
	case ComponentType::CAMERA:
		Camera::SaveComponentCamera(config, component);
		break;
	case ComponentType::MESH:
		//all you need is component type and ID, and you already have that
		break;
	case ComponentType::MATERIAL:
		Texture::SaveComponentMaterial(config, component);
		break;
	case ComponentType::TRANSFORM:
		//we're already saving it as an array
		break;
	case ComponentType::AUDIO_LISTENER:
		Audio::SaveComponentAudioListener(config, component);
		break;
	case ComponentType::AUDIO_SOURCE:
		Audio::SaveComponentAudioSource(config, component);
		break;
	case ComponentType::REVERB_ZONE:
		Audio::SaveComponentReverbZone(config, component);
		break;
	case ComponentType::CONTROL:
		Controller::SaveComponentController(config, component);
		break;
	default:
		//how did you even get here smh
		LOG("[error] Trying to save component with ID %i from Game Object %s but the type is invalid", component->ID, component->owner->GetName());
		break;
	}

}

void Importer::Mesh::ImportRMesh(aiMesh* fbxMesh, ResourceMesh& resToFill)
{
	//UID should already be in place from Import Model

	LOG("----------Importing mesh '%s'----------", (char*)fbxMesh->mName.C_Str());


	// copy vertices
	//newMesh.numVertex = mesh->mNumVertices;

	if (fbxMesh != nullptr && fbxMesh->mNumVertices > 0)
	{
		resToFill.vertices.reserve(fbxMesh->mNumVertices * 3);


		for (int j = 0; j < fbxMesh->mNumVertices; j++)
		{
			resToFill.vertices.push_back(fbxMesh->mVertices[j].x);
			resToFill.vertices.push_back(fbxMesh->mVertices[j].y);
			resToFill.vertices.push_back(fbxMesh->mVertices[j].z);
		}
		// copy faces
		if (fbxMesh->HasFaces())
		{
			//newMesh.numIndex = mesh->mNumFaces * 3;
			//newMesh.index = new unsigned int[newMesh.numIndex]; // assume each face is a triangle
			resToFill.indices.reserve(fbxMesh->mNumFaces * 3);
			resToFill.indices.resize(fbxMesh->mNumFaces * 3);

			for (int j = 0; j < fbxMesh->mNumFaces; j++)
			{

				if (fbxMesh->mFaces[j].mNumIndices != 3)
				{
					LOG("[waring] geometry face with != 3 indices!");
				}
				else
				{
					//indices.push_back(mesh->mFaces[j].mIndices[0]);
					//indices.push_back(mesh->mFaces[j].mIndices[1]);
					//indices.push_back(mesh->mFaces[j].mIndices[2]);

					memcpy(&resToFill.indices[j * 3], fbxMesh->mFaces[j].mIndices, 3 * sizeof(unsigned int));


				}

			}
		}


		resToFill.texCoords.reserve(fbxMesh->mNumVertices * 2); //there are 2 floats for every index
		LOG("Importing mesh texture coordinates");
		for (int j = 0; j < fbxMesh->mNumVertices; j++)
		{
			//copy TextureCoords
			if (fbxMesh->mTextureCoords[0])
			{

				resToFill.texCoords.push_back(fbxMesh->mTextureCoords[0][j].x);
				resToFill.texCoords.push_back(fbxMesh->mTextureCoords[0][j].y);
			}
			else
			{
				LOG("[warning]No texture coordinates found");
				resToFill.texCoords.push_back(0.0f);
				resToFill.texCoords.push_back(0.0f);
			}
		}


		//copy normals
		if (fbxMesh->HasNormals())
		{
			LOG("Importing normals");
			resToFill.normals.reserve(fbxMesh->mNumVertices * 3);
			for (int j = 0; j < fbxMesh->mNumVertices; j++)
			{

				resToFill.normals.push_back(fbxMesh->mNormals[j].x);
				resToFill.normals.push_back(fbxMesh->mNormals[j].y);
				resToFill.normals.push_back(fbxMesh->mNormals[j].z);
			}
			resToFill.GenerateSmoothedNormals();
		}
		else
			LOG("[warning]Mesh has no normals!");

		resToFill.GenerateBuffers();
	}
}

void Importer::Audio::SaveComponentAudioListener(Config& config, Component* audioListener)
{
	AudioListener* listener = (AudioListener*)audioListener;
	//TODO complete this method
	config.SetBool("IsListener", listener->GetIsListener());

}

void Importer::Audio::SaveComponentAudioSource(Config& config, Component* audioSource)
{
	AudioSource* source = (AudioSource*)audioSource;
	//TODO complete this method
	config.SetNumber("Volume", source->GetVolume());
	config.SetNumber("SecondsToNextMusic", source->GetSecondsToChangeMusic());
	config.SetNumber("UserPitch", source->GetUserPitch());
	Config_Array eventsArray = config.SetArray("Events");
	const std::vector<AudioEvent*> events = source->GetEvents();
	for (int i = 0; i < events.size(); i++)
	{
		Config& c = eventsArray.AddNode();
		c.SetString("Name", events[i]->GetEventName().c_str());
		c.SetNumber("Type", (int)events[i]->GetType());
	}

}

void Importer::Audio::SaveComponentReverbZone(Config& config, Component* reverbZone)
{
	ModuleReverbZone* reverb = (ModuleReverbZone*)reverbZone;
	float3 dimensions = reverb->GetDimensions();
	config.SetNumber("DimensionsX", dimensions.x);
	config.SetNumber("DimensionsY", dimensions.y);
	config.SetNumber("DimensionsZ", dimensions.z);
	config.SetString("TargetBus", reverb->targetBus.c_str());
	config.SetNumber("RevValue", reverb->revValue);

}

void Importer::Controller::SaveComponentController(Config& config, Component* controller)
{
	ModuleControl* control = (ModuleControl*)controller;
	config.SetNumber("Speed", control->GetSpeed());
}
