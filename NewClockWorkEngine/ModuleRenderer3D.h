#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "ModuleImporter.h"


typedef void* SDL_GLContext;
struct Mesh;
typedef unsigned int GLuint;
typedef signed char GLbyte;
class ModuleMesh;

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void SetDepthtest(bool state);
	void SetCullface(bool state);
	void SetLighting(bool state);
	void SetColormaterial(bool state);
	void SetTexture2D(bool state);
	void SetCubemap(bool state);
	void SetPolygonssmooth(bool state);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};