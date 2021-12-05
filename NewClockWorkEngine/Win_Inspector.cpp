#include "Win_Inspector.h"
#include "OpenGL.h"
#include <stdio.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "glew.h"
#include "SDL_opengl.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleComponent.h"
#include "GameObject.h"
#include "ModuleTransform.h"

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

	if (App->scene_intro->selected != nullptr)
	{
		std::vector<ModuleComponent*> vector = App->scene_intro->selected->GetComponents();
		std::vector<ModuleComponent*>::iterator item = vector.begin();
		for (; item != vector.end(); ++item)
		{
			(*item)->DrawInspector();
		}
	}

	ImGui::End();

	

}

void Win_Inspector::CleanUp()
{
}