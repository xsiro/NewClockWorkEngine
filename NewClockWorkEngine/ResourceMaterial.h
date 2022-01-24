#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Resources.h"
#include <string>

class ResourceMaterial:public Resource
{
public:
	ResourceMaterial(unsigned int UID);
	~ResourceMaterial();

	void GenTextureFromName(unsigned int ilImageName);
	void DestroyTexture();
	unsigned int GetTextureID()const;
	bool UnloadFromMemory()override;

public:
	unsigned int width, height;
	int format;
	int depth;
	int sizeInBytes;
	unsigned int bpp;

	unsigned int idTexture;
	int ilImageID; //this is just for saving, after that this variable is deleted
};

#endif // !__TEXTURE_H__
