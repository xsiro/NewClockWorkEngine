#include "Application.h"
#include "ModuleMaterial.h"
#include "imgui.h"
#include "Globals.h"
#include "ModuleImporter.h"
#include "ModuleRenderer3D.h"
#include "ModuleTransform.h"
#include "GameObject.h"
#include "ModuleComponent.h"
#include "ResourceMaterial.h"
#include "Config.h"

#include "SDL/include/SDL_opengl.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleMaterial::ModuleMaterial(GameObject* owner) : ModuleComponent(ComponentType::Material, owner)
{

}

ModuleMaterial::ModuleMaterial(GameObject* owner, const char* path, ResourceMaterial* texture = nullptr) : ModuleComponent(ComponentType::Material, owner), material(texture), path(path)
{

}

ModuleMaterial::~ModuleMaterial()
{

}

void ModuleMaterial::Update()
{

}

void ModuleMaterial::CleanUp()
{
	delete material;
}
void ModuleMaterial::DrawInspector()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::Text("Path: %s", path);
		ImGui::Text("Texture height: %d", material->GetHeight());
		ImGui::Text("Texture width: %d", material->GetWidth());
		if (ImGui::Checkbox("DrawTexture", &drawTexture)) {}
	}
}

void ModuleMaterial::OnSave(ConfigNode* node)
{

}

const char* ModuleMaterial::GetPath()const
{
	return path;
}

ResourceMaterial* ModuleMaterial::GetTexture() const
{
	return material;
}

bool ModuleMaterial::IsEnabled() const
{
	return drawTexture;
}

void ModuleMaterial::SwitchEnabledTexture()
{
	active = !active;
}