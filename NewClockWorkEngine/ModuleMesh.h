#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"
#include "ModuleImporter.h"


class GameObject;
class ResourceMesh;
class ModuleComponent;
typedef unsigned int GLuint;
typedef unsigned char GLubyte;

class ModuleMesh : public ModuleComponent
{
public:

	ModuleMesh(GameObject* owner);
	ModuleMesh(GameObject* owner, char* path, ResourceMesh* mesh);
	~ModuleMesh();

	void Update() override;
	void CleanUp() override;
	void DrawInspector() override;
	ResourceMesh* GetMesh() const;

	void DrawMesh();
	char* GetPath()const;

	const AABB& GetAABB() const;
	const OBB& GetOBB() const;
	static inline ComponentType GetType() { return ComponentType::Mesh; };


private:

	ResourceMesh* mesh = nullptr;
	char* path = nullptr;
	bool drawVertexNormals = false;

public:

	AABB aabb;
	OBB obb;
};

