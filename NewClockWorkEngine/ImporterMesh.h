#ifndef _IMPORTER_MESH_H_
#define _IMPORTER_MESH_H_

#include "Globals.h"

class ResourceMesh;

class aiMesh;

namespace ImporterMesh
{
	void Init();
	void CleanUp();

	void Import(const aiMesh* aimesh, ResourceMesh* mesh);
	uint64 Save(ResourceMesh* mesh, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceMesh* mesh, uint size);
}

#endif

