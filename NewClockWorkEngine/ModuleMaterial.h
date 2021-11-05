#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"

class GameObject;
typedef unsigned int GLuint;
typedef unsigned char GLubyte;

class ModuleMaterial : public ModuleComponent
{
public:

	ModuleMaterial();
	~ModuleMaterial();
	virtual void Update();

public:
	GLuint Gl_Tex;
};