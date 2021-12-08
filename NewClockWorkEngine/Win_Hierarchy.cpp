#include "Win_Hierarchy.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"

Win_Hierarchy::Win_Hierarchy(bool _active) : Window(_active)
{}


Win_Hierarchy::~Win_Hierarchy()
{}

void Win_Hierarchy::Init()
{

}

void Win_Hierarchy::Draw()
{
	if (!active)
		return;
	if (!ImGui::Begin("Hierarchy", &active))
	{
		ImGui::End();
		return;
	}


	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	std::vector<GameObject*>::iterator item = App->scene_intro->game_objects.begin();
	for (; item != App->scene_intro->game_objects.end(); ++item)
		GameObjectsHierarchy((*item));

	ImGui::End();
}

void Win_Hierarchy::CleanUp()
{
}

void Win_Hierarchy::GameObjectsHierarchy(GameObject* object)
{

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (!object->children.empty())
	{
		bool node_open = ImGui::TreeNodeEx(object->GetName(), baseFlags);
		if (ImGui::IsItemClicked())
			App->scene_intro->SetSelectedObject(object);

		if (node_open)
		{
			std::vector<GameObject*>::iterator child = object->children.begin();
			for (; child != object->children.end(); ++child)
			{
				GameObjectsHierarchy((*child));
			}
			ImGui::TreePop();
		}
	}
	else
	{
		ImGuiTreeNodeFlags finalFlags = baseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		bool node_open = ImGui::TreeNodeEx(object->GetName(), finalFlags);
		if (ImGui::IsItemClicked())
			App->scene_intro->SetSelectedObject(object);
	}

}

