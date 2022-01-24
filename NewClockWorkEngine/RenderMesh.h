#ifndef __RENDER_MESH_H__
#define __RENDER_MESH_H__


//#include "glmath.h" // cannot forward declare mat4x4
#include "MathGeoLib/include/MathGeoLib.h"
#include "Color.h"

class ModuleMesh;
class ModuleMaterial;
class ResourceMesh;
enum class MeshDrawMode;

class RenderMesh
{
public:
	RenderMesh(ModuleMesh* mesh,ModuleMaterial*material, float4x4 gTransform,Color color=Color(1.0f,1.0f,1.0f));
	~RenderMesh();
	void Draw(MeshDrawMode sceneMaxDrawMode);

private:
	void DrawVertexNormals(ResourceMesh*m);
	void DrawFacesNormals(ResourceMesh* m);
	void DrawBuffers(ResourceMesh* m);

private:
	ModuleMesh* mesh;
	ModuleMaterial* material;

	float4x4 transform;
	Color color;


};

#endif // !__RENDER_MESH_H__

