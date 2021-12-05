#include "Win_Hierarchy.h"
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
	ImGui::Begin("Hierarchy", &active);

	if (ImGui::Button("Delete"))
	{
		App->scene_intro->CleanUp();
	}
	GameObjectsHierarchy();
	ImGui::End();
}

void Win_Hierarchy::CleanUp()
{
}

void Win_Hierarchy::GameObjectsHierarchy()
{

	if (ImGui::TreeNode("GameObject")) {
		for (size_t i = 0; i < App->scene_intro->game_objects.size(); i = i + 2)
		{
			if (App->scene_intro->game_objects[i]->parent != nullptr) continue;

			if (ImGui::TreeNodeEx(App->scene_intro->game_objects[i]->name.c_str(), ImGuiTreeNodeFlags_Leaf)) {
				if (ImGui::IsItemClicked()) {
					App->scene_intro->selected = App->scene_intro->game_objects[i];
				}
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

}

