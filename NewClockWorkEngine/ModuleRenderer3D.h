#ifndef __MODULE_RENDERER__
#define __MODULE_RENDERER__


#include "Module.h"
//#include "Globals.h"
#include "Light.h" //incuded due to an array declared here
#include <vector>
#include "RenderMesh.h"
#include "RenderBox.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"
#include "ModuleCamera.h" //TODO forward declare this



#define MAX_LIGHTS 8
#define INDEX_CUBE 36

//class ModuleCamera;
class ResourceMesh;


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
	void GenerateBuffers(int width, int height);
	void Draw3D();
	void DrawOutline();


	void AddMeshToDraw(ModuleMesh* mesh, ModuleMaterial* material, float4x4 gTransform,bool isSelected);
	void AddMeshToStencil(ModuleMesh* mesh, float4x4 gTransform,Color color=Color(1.0f,1.0f,1.0f));
	void AddBoxToDraw(std::vector<float3> corners,Color c= Color(1.0f, 1.0f, 1.0f));

	bool IsInsideFrustum(std::vector<float3>& points);

	void SetCamRay(LineSegment line);

private:
	void RenderMeshes();
	void RenderSelectedMeshes();
	void RenderStencil();
	void RenderAABBs();
	void DrawGrid();
	void DrawDebugRay();

	//sets all the config options(depth testing, cull faces,etc...) to their bool values
	void SetGLRenderingOptions();

	bool ExpandMeshVerticesByScale(ResourceMesh& m, float newScale);//returns false if scaling cannot be done

private:
	unsigned int exampleMeshIdentifier;
	unsigned int indexBind;
	unsigned int vertexBind;
	int nVertex;
	int indexSize;
	std::vector<RenderMesh> drawMeshes;
	std::vector<RenderMesh> drawSelectedMeshes;
	std::vector<RenderMesh> drawStencil;
	std::vector<RenderBox> drawAABBs;
	std::vector<ModuleMesh*> stencilMeshes;
	LineSegment rayLine;

public:
	Light lights[MAX_LIGHTS];

	SDL_GLContext context;
	

	unsigned int frameBuffer;
	unsigned int renderTex;
	unsigned int depthBuffer;

	float gridLength;
	float outlineScale;
	LCG seed;

	//rendering config bools
	bool depthTesting;
	bool cullFace;
	bool lighting;
	bool colorMaterial;
	bool texture2D;
	bool drawGrid;
	bool drawDebugRay;
	bool showDepth;
	bool displayAABBs;
	ModuleCamera* activeCam;//culling camera
};
#endif // !__MODULE_RENDERER__