#include "Application.h"
#include "ModuleMaterial.h"
#include "imgui.h"
#include "Globals.h"
#include "ModuleImporter.h"

#include "SDL/include/SDL_opengl.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>


#pragma comment (lib, "glu32.lib") 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "Glew/libx86/glew32.lib")

ModuleMaterial::ModuleMaterial(GameObject* owner) : ModuleComponent(ComponentType::Material, owner)
{

}

ModuleMaterial::ModuleMaterial(GameObject* owner, char* path, Material* texture = nullptr) : ModuleComponent(ComponentType::Material, owner), material(texture), path(path)
{

}

ModuleMaterial::~ModuleMaterial()
{
}
void ModuleMaterial::Update()
{

}

void ModuleMaterial::DrawInspector()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::Text("Path: %s", path);
		ImGui::Text("Texture height: %d", material->height);
		ImGui::Text("Texture width: %d", material->width);
		if (ImGui::Checkbox("DrawTexture", &drawTexture)) {}
	}
}

char* ModuleMaterial::GetPath()const
{
	return path;
}

Material* ModuleMaterial::GetTexture() const
{
	return material;
}

bool ModuleMaterial::IsEnable()
{
	return drawTexture;
}

void ModuleMaterial::SwitchEnabledTexture()
{
	active = !active;
}