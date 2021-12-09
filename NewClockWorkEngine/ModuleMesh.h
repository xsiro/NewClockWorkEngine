#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"
#include "ModuleImporter.h"


class GameObject;
class ModuleMaterial;
typedef unsigned int GLuint;
typedef unsigned char GLubyte;

class ModuleMesh : public ModuleComponent
{
public:

	ModuleMesh(GameObject* owner);
	ModuleMesh(GameObject* owner, char* path, Mesh* mesh);
	~ModuleMesh();

	void Update() override;
	void CleanUp() override;
	void DrawInspector() override;
	Mesh* GetMesh() const;

	void DrawMesh();
	char* GetPath()const;

	const AABB& GetAABB() const;
	const OBB& GetOBB() const;


private:

	Mesh* mesh = nullptr;
	char* path = nullptr;
	bool drawVertexNormals = false;

public:

	AABB aabb;
	OBB obb;
};

