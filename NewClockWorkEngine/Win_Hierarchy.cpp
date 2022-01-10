#include "Win_Hierarchy.h"
#include "Window.h"
#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_opengl3.h"
#include "imgui/include/imgui_impl_sdl.h"
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

	if (ImGui::Button("Delete"))
	{
		App->scene_intro->selected->CleanUp();
	}

	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	std::vector<GameObject*>::iterator item = App->scene_intro->rootObject->children.begin();
	for (; item != App->scene_intro->rootObject->children.end(); ++item)
		GameObjectsHierarchy((*item));

	if (ImGui::IsWindowHovered())
		App->gui->mouseHovered = true;
	ImGui::End();
}

void Win_Hierarchy::CleanUp()
{
}

void Win_Hierarchy::GameObjectsHierarchy(GameObject* object)
{

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;


	if (object->children.empty())
		baseFlags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx(object->GetName(), baseFlags))
	{
		if (ImGui::IsItemClicked())
			App->scene_intro->SetSelectedObject(object);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Dragged GO", object, sizeof(GameObject));
			ImGui::Text("%s", object->GetName());
			draggedObject = object;

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Dragged GO"))
			{
				//do thing with object
				LOG("Dropped %s", draggedObject->GetName(), object->GetName());
			}

			//LOG("Dropped %s", object->GetName());
			ImGui::EndDragDropTarget();
		}

		if (!object->children.empty())
		{
			std::vector<GameObject*>::iterator child = object->children.begin();
			for (; child != object->children.end(); ++child)
			{
				GameObjectsHierarchy((*child));
			}
		}

		ImGui::TreePop();
	}

}

