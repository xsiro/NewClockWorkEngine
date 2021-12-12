#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"
#include "ModuleImporter.h"
#include "MathGeoLib/src/MathGeoLib.h"

class GameObject;
class ResourceMesh;
class ModuleComponent;
class ConfigNode;

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
	void OnSave(ConfigNode* node);
	ResourceMesh* GetMesh() const;

	static inline ComponentType GetType() { return ComponentType::Mesh; };

	void DrawMesh();

	char* GetPath()const;

	const AABB& GetAABB() const;
	const OBB& GetOBB() const;

private:

	ResourceMesh* mesh = nullptr;
	char* path = nullptr;
	bool drawVertexNormals = false;
	bool drawAABB = false;


public:

	AABB aabb;
	OBB obb;
};

