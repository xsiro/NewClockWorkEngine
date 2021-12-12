#include "Resource.h"
#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial()
{

}

ResourceMaterial::~ResourceMaterial()
{

}

void ResourceMaterial::SetId(uint id)
{
	this->id = id;
}

void ResourceMaterial::SetHeight(uint height)
{
	this->height = height;
}

void ResourceMaterial::SetWidth(uint width)
{
	this->width = width;
}


void ResourceMaterial::SetPath(const char* path)
{
	this->path = path;
}

uint ResourceMaterial::GetId() const
{
	return id;
}

uint ResourceMaterial::GetHeight() const
{
	return height;
}

uint ResourceMaterial::GetWidth() const
{
	return width;
}