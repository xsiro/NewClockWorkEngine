#pragma once
#include "Resource.h"

class Resource;

class ResourceMaterial : public Resource
{
public:

	ResourceMaterial();
	~ResourceMaterial();

	void SetId(uint id);
	void SetHeight(uint height);
	void SetWidth(uint width);
	void SetPath(const char* path);
	void SetTexture(ResourceMaterial* texture);

	uint GetId() const;
	uint GetHeight() const;
	uint GetWidth() const;

private:

	uint width = 0;
	uint height = 0;

	uint id = 0; 

	const char* path = nullptr;

};

