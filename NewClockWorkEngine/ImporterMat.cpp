#include "ImporterMat.h"
#include "Application.h"
#include "Timer.h"
#include "ResourceMat.h"
#include "FileSys.h"
#include "JSON.h"
#include "Assimp/include/material.h"

void ImporterMat::Import(const aiMaterial* aimaterial, ResourceMat* material)
{
	Timer timer;
	timer.Start();

	aiString path;
	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
	aiColor3D aiDiffuseColor;
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor);

	if (path.length > 0)
	{
		std::string file_path = material->assetsFile;
		path = FileSys::GetFile(path.C_Str());
		file_path = FindTexture(path.C_Str(), material->assetsFile.c_str());

		if (file_path.size() > 0)
		{
			std::string meta_file = App->res->GenerateMetaFile(file_path.c_str());
			if (!FileSys::Exists(meta_file.c_str()))
				material->diffuseTextureUID = App->res->ImportFile(file_path.c_str());
			else
				material->diffuseTextureUID = App->res->Find(file_path.c_str());
		}

		material->diffuseColor.r = aiDiffuseColor.r;
		material->diffuseColor.g = aiDiffuseColor.g;
		material->diffuseColor.b = aiDiffuseColor.b;


	}
}

uint64 ImporterMat::Save(ResourceMat* material, char** fileBuffer)
{
	JSON base_object;
	base_object.AddInt("diffuseTexture", material->diffuseTextureUID);

	base_object.AddColor("diffuseColor", material->diffuseColor);

	char* buffer;
	uint size = base_object.Save(&buffer);
	*fileBuffer = buffer;

	return size;
}

bool ImporterMat::Load(const char* fileBuffer, ResourceMat* material, uint size)
{
	bool ret = true;
	Timer timer;
	timer.Start();

	JSON material_data(fileBuffer);
	material->diffuseTextureUID = material_data.GetInt("diffuseTexture", 0);

	if (material->diffuseTextureUID != 0)
		App->res->LoadResource(material->diffuseTextureUID, ResourceType::RESOURCE_TEXTURE);

	material->diffuseColor = material_data.GetColor("diffuseColor");

	material_data.Release();
	return ret;
}

std::string ImporterMat::FindTexture(const char* texture_name, const char* model_directory)
{
	std::string path;
	FileSys::SplitFilePath(model_directory, &path);
	std::string texture_path = path + texture_name;

	//Check if the texture is in the same folder
	if (FileSys::Exists(texture_path.c_str()))
	{
		return texture_path.c_str();
	}
	else
	{
		//Check if the texture is in a sub folder
		texture_path = path + "Textures/" + texture_name;
		if (FileSys::Exists(texture_path.c_str()))
		{
			return texture_path.c_str();
		}
		else
		{
			//Check if the texture is in the root textures folder
			texture_path = std::string("Assets/Textures/") + texture_name;
			if (FileSys::Exists(texture_path.c_str()))
			{
				return texture_path.c_str();
			}
		}
	}

	texture_path.clear();
	return texture_path;
}

bool ImporterMat::DeleteTexture(const char* material_library_path)
{
	bool ret = true;

	char* buffer = nullptr;
	FileSys::Load(material_library_path, &buffer);

	JSON material_data(buffer);

	int diffuseTextureUID = material_data.GetInt("diffuseTexture");
	const char* texture_library_path = App->res->Find(diffuseTextureUID);

	if (texture_library_path != nullptr)
	{
		FileSys::Delete(texture_library_path);
		App->res->ReleaseResource(diffuseTextureUID);
		App->res->ReleaseResourceData(diffuseTextureUID);
	}
	else
		LOG_WARNING("Texture: %s could not be deleted. Not found", material_library_path);

	material_data.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

const char* ImporterMat::ExtractTexture(const char* material_library_path)
{
	char* buffer = nullptr;
	FileSys::Load(material_library_path, &buffer);

	JSON material_data(buffer);

	int diffuseTextureUID = material_data.GetInt("diffuseTexture");
	const char* texture_library_path = App->res->Find(diffuseTextureUID);

	material_data.Release();
	RELEASE_ARRAY(buffer);

	return texture_library_path;
}
