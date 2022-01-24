#pragma once

#include "Component.h"
#include <vector>

#include "MathGeoLib/include/Geometry/AABB.h"

class ResourceMesh;

class ModuleMesh:public Component
{
public:
	ModuleMesh(GameObject* owner,unsigned int ID);
	~ModuleMesh();
	void SetNewResource(unsigned int resourceUID)override;
	ResourceMesh* GetMesh();
	unsigned int GetResourceID()override;
	void OnEditor();

	AABB GetAABB()const;

	void SetTemporalMesh(ResourceMesh* newTempMesh);
	ResourceMesh* GetTemporalMesh();
	void DeleteTemporalMesh();

public:
	float normalVertexSize;
	float normalFaceSize;
	int normalDrawMode;
	int meshDrawMode;
private:
	unsigned int resourceID;
	AABB localAABB;

	ResourceMesh* temporalRMesh;
};



