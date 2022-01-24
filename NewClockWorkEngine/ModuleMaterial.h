#pragma once

#include "Component.h"
#include <string>
#include "Color.h"

class ResourceMaterial;
class ModuleMaterial:public Component
{
public:
	ModuleMaterial(GameObject* owner,unsigned int ID,Color color=Color(1.0f,1.0f,1.0f));
	~ModuleMaterial();

	void SetNewResource(unsigned int resourceUID)override;
	unsigned int GetResourceID()override;

	bool HasTexture();
	bool HasCheckers()const;
	unsigned int GetTextureID();
	ResourceMaterial* GetTexture();
	unsigned int GetCheckersID()const;

	void OnEditor();

private:


	void DestroyCheckers();
	void GenDefaultTexture();


public:
	bool usingCkeckers;


	Color matCol;

private:
	unsigned int idCheckers;
	int size = 200; 
	unsigned int resourceID;

};

