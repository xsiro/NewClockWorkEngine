#include "Globals.h"
#include "Application.h"
#include "glew.h"
#include "ModuleMesh.h"
#include "ModuleMaterial.h"
#include "ModuleTransform.h"
#include "imgui.h"
#include "ModuleGui.h"
#include "ModuleImporter.h"
#include "ModuleComponent.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h"
#include "ResourceMesh.h"
#include "Resource.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib") 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew/libx86/glew32.lib")

ModuleMesh::ModuleMesh(GameObject* owner) : ModuleComponent(ComponentType::Mesh, owner)
{

}

ModuleMesh::ModuleMesh(GameObject* owner, char* path, ResourceMesh* mesh = nullptr) : ModuleComponent(ComponentType::Mesh, owner), mesh(mesh), path(path)
{
}

ModuleMesh::~ModuleMesh()
{

}

void ModuleMesh::Update()
{
	DrawMesh();
}

void ModuleMesh::CleanUp()
{
	delete mesh;
}

void  ModuleMesh::DrawInspector()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::Text("Path: %s", path);
		ImGui::Text("Vertices: %d", mesh->buffersSize[Mesh::vertex]);
		ImGui::Checkbox("Active", &this->active);
		ImGui::Checkbox("Draw Vertex Normals", &drawVertexNormals);
	}
}

void ModuleMesh::DrawMesh()
{
	if (!this->active)
		return;

	if (owner->GetComponent<ModuleMaterial>() != nullptr)
	{
		if (owner->GetComponent<ModuleMaterial>()->IsEnabled())
		{


			App->renderer3D->DrawMesh(mesh, owner->transform->GetGlobalTransform(), owner->GetComponent<ModuleMaterial>()->GetTexture(), drawVertexNormals, owner);


			return;
		}
	}

	App->renderer3D->DrawMesh(mesh, owner->transform->GetGlobalTransform(), nullptr, drawVertexNormals, owner);
}

char* ModuleMesh::GetPath()const
{
	return path;
}

const AABB& ModuleMesh::GetAABB() const
{
	return aabb;
}

const OBB& ModuleMesh::GetOBB() const
{
	return obb;
}
ResourceMesh* ModuleMesh::GetMesh() const
{
	return mesh;
}