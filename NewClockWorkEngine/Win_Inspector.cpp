#include "Win_Inspector.h"
#include "OpenGL.h"
#include <stdio.h>
#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_opengl3.h"
#include "imgui/include/imgui_impl_sdl.h"
#include "glew/include/glew.h"
#include "sdl/include/SDL_opengl.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleComponent.h"
#include "GameObject.h"
#include "ModuleTransform.h"
#include "ModuleRenderer3D.h"

Win_Inspector::Win_Inspector(bool _active) : Window(_active)
{
	depthtest = false;
	cullface = false;
	lighting = false;
	material = false;
	cubemap = true;
	polygonssmooth = false;

	wireframe = false;
	vertexlines = false;
	facelines = false;
	check = false;
}


Win_Inspector::~Win_Inspector()
{}

void Win_Inspector::Init()
{

}

void Win_Inspector::Draw()
{
	if (!active)
		return;

	if (!ImGui::Begin("Inspector", &active))
	{
		ImGui::End();
		return;
	}
	if (ImGui::CollapsingHeader("Render"))
	{
		if (ImGui::Checkbox("Wireframe Mode", &App->renderer3D->wireframeMode)) {}
		if (ImGui::Checkbox("GL_CULL_FACE", &App->renderer3D->SetCullface)) { App->renderer3D->SwitchCullFace(); }
		if (ImGui::Checkbox("GL_DEPTH_TEST", &App->renderer3D->SetDepthtest)) { App->renderer3D->SwitchDepthTest(); }
		if (ImGui::Checkbox("GL_LIGHT", &App->renderer3D->SetLighting)) { App->renderer3D->SwitchLighting(); }
		if (ImGui::Checkbox("GL_COLOR_MATERIAL", &App->renderer3D->SetColormaterial)) { App->renderer3D->SwitchColorMaterial(); }
		if (ImGui::Checkbox("GL_TEXTURE_2D", &App->renderer3D->SetTexture2D)) { App->renderer3D->SwitchTexture2d(); }
	}
		
	

	if (App->scene_intro->selected != nullptr)
	{
		std::vector<ModuleComponent*> vector = App->scene_intro->selected->GetComponents();
		std::vector<ModuleComponent*>::iterator item = vector.begin();
		for (; item != vector.end(); ++item)
		{
			(*item)->DrawInspector();
		}
	}

	if (ImGui::IsWindowHovered())
		App->gui->mouseHovered = true;
	ImGui::End();

	

}

void Win_Inspector::CleanUp()
{
}