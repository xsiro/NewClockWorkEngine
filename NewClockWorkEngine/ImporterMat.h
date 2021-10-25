#ifndef _IMPORTER_MAT_H_
#define _IMPORTER_MAT_H_

#include "Globals.h"
#include <string>
class aiMaterial;
class ResourceMat;

namespace ImporterMat
{
	void Import(const aiMaterial* aimaterial, ResourceMat* material);
	uint64 Save(ResourceMat* material, char** fileBuffer);
	bool Load(const char* fileBuffer, ResourceMat* material, uint size);

	std::string FindTexture(const char* texture_name, const char* model_directory);
	bool DeleteTexture(const char* material_library_path);
	const char* ExtractTexture(const char* material_library_path);
}

#endif

