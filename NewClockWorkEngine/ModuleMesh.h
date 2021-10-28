#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"
#include "Moduleimporter.h"

class GameObject;

class ModuleMesh : public ModuleComponent
{
public:

	ModuleMesh();
	~ModuleMesh();

	void CreateCubeDirect();
	void CreateCubeVertex();
	void CreateCubeIndex();
	void CreatePyramid();
	void CreateSphere(float radius, unsigned int rings, unsigned int sectors);
	void CreateCylinder(float radius, float height, int sides);

	void RenderFBX();
	void LoadFBXBuffer();
	void DrawVertexNormalLines();
	void DrawFaceNormalLines();
	void LoadingTextures();


public:

	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint	id_normals = 0;
	uint	num_normals = 0;
	float* normals = NULL;

	uint	id_colors = 0;
	uint	num_colors = 0;
	float* colors = NULL;

	uint	id_texcoords = 0;
	uint	num_texcoords = 0;
	float* texcoords = nullptr;
	uint image_id;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;


};

