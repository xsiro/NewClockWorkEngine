#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"


typedef unsigned int GLuint;
typedef unsigned char GLubyte;
class ModuleComponent;
struct Material;

class ModuleMaterial : public ModuleComponent
{
public:

	ModuleMaterial(GameObject* owner);
	ModuleMaterial(GameObject* owner, char* path, Material* mesh);;
	~ModuleMaterial();
	void Update() override;
	void CleanUp() override;
	void DrawInspector() override;

	char* GetPath()const;
	Material* GetTexture() const;
	bool IsEnable();
	void SwitchEnabledTexture();

public:
	GLuint Gl_Tex;

private:

	Material* material = nullptr;
	char* path = "";
	bool drawTexture = true;

};