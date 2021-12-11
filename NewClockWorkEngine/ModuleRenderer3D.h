#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "ModuleImporter.h"
#include "MathGeoLib/src/MathGeoLib.h"
#include "SDL.h"
#include <vector>


struct Mesh;

template <typename Box>
struct RenderBox
{
	RenderBox(const Box* box, const Color& color) : box(box), color(color)
	{}

	const Box* box;
	Color color;
};

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	void OnResize(int width, int height);
	void DrawMesh(Mesh* mesh, float4x4 transform, uint textureId, bool drawVertexNormals);
	void DrawVertexNormals(Mesh* mesh);
	void GenerateBuffers(Mesh* newMesh);
	void CreateChekerTexture();

	void SwitchCullFace();
	void SwitchDepthTest();
	void SwitchLighting();
	void SwitchTexture2d();
	void SwitchColorMaterial();
	void CreateAABB(const AABB& box, const Color& color);
	void CreateOBB(const OBB& box, const Color& color);
	void DrawBB();


public:

	bool SetDepthtest;
	bool SetCullface;
	bool SetLighting;
	bool SetColormaterial;
	bool SetTexture2D;
	bool SetCubemap=false;
	//bool SetPolygonssmooth;
	bool wireframeMode;

	uint checkersId;
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	std::vector<RenderBox<AABB>> aabb;
	std::vector<RenderBox<OBB>> obb;
};