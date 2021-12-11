#pragma once
#include "MathGeoLib/src/MathGeoLib.h"
#include "Resource.h"

class Resource;

class ResourceMesh : public Resource
{
public:

	ResourceMesh();
	~ResourceMesh();


	enum Buffers
	{
		index,
		vertex,
		normal,
		texture,
		maxBuffers
	};

public:
	uint buffersId[ResourceMesh::maxBuffers];
	uint buffersSize[ResourceMesh::maxBuffers];

	uint* indices = nullptr;
	float* vertices = nullptr;
	float* normals = nullptr;
	float* textureCoords = nullptr;

	const char* path;

	AABB aabb;
};
