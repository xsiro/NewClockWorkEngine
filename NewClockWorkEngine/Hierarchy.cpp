#include "Hierarchy.h"
#include "imgui/imgui.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"

//value ret tied to SHow Hierarchy
bool ShowHierarchyTab()
{
	bool ret = true;

	if (App->scene->root != nullptr)
	{
		SeekMyChildren(App->scene->root);
	}
	return ret;
}

//This is the recursive func that makes the hierarchy
void SeekMyChildren(GameObject* myself)
{
	bool droppedItem = false;
	ImGuiTreeNodeFlags TreeNodeEx_flags = ImGuiTreeNodeFlags_OpenOnArrow; //yeah, we have to set it each iteration. ImGui Badness

	if (myself->children.size() == 0)
		TreeNodeEx_flags = ImGuiTreeNodeFlags_Leaf;

	if (myself == App->scene->root)
	{
		for (int i = 0; i < myself->children.size(); i++)
		{
			SeekMyChildren(myself->children[i]);
		}
	}
	else
	{


		ImGui::PushStyleColor(ImGuiCol_Text, ChooseMyColor(myself));

		bool open = ImGui::TreeNodeEx(myself->GetName().c_str(), TreeNodeEx_flags);




		//==========================================================================================
		// Our buttons are both drag sources and drag targets here!
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			// Set payload to carry the object pointer
			ImGui::SetDragDropPayload("OBJ_ID", &myself->ID, sizeof(unsigned int));

			// Display preview (could be anything, e.g. when dragging an image we could decide to display
			// the filename and a small preview of the image, etc.)

			ImGui::Text("%s", myself->GetName().c_str());

			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("OBJ_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(unsigned int));
				//int payload_n = *(const int*)payload->Data;

				GameObject* newObj = nullptr;
				App->scene->root->GetChildWithID(*(const unsigned int*)payload->Data, newObj);
				GameObject* newParent = nullptr;
				App->scene->root->GetChildWithID(myself->ID, newParent);

				GameObject* isParent = nullptr;
				newObj->GetChildWithID(newParent->ID, isParent);

				if (!isParent)
					newObj->ChangeParent(newParent);


				/*newParent->children.push_back(newObj);
				newObj->parent = newParent;*/

				/*if (mode == Mode_Move)
				{
					names[n] = names[payload_n];
					names[payload_n] = "";
				}*/
				droppedItem = true;

			}
			ImGui::EndDragDropTarget();
		}

		//==========================================================================================



		if (ImGui::IsItemHovered())//we use itemHovered + mouse click check because we want the mouse to perform the action on key up, otherwise it would select an item when dropping a payload
		{
			//this if is redundant, check is already done inside setSelectedGameObject()
			/*if (!App->scene->GetSelectedGameObject().empty())
			{
				App->scene->GetSelectedGameObject().back()->focused = false;
			}*/

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP&& !droppedItem)
			{

				if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
				{
					App->scene->SetSelectedGameObject(myself, true);
				}
				else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
				{
					App->scene->RemoveGameObjFromSelected(myself);
				}
				else
				{
					App->scene->SetSelectedGameObject(myself);
				}
			}



		}

		if (open == true)
		{

			for (int i = 0; i < myself->children.size(); i++)
			{
				SeekMyChildren(myself->children[i]);
			}
			ImGui::TreePop();
		}

		ImGui::PopStyleColor();
	}
}

//Quick func that return the color vector based on IsActive
ImVec4 ChooseMyColor(GameObject* myself)
{
	ImVec4 activeColor ACTIVE_COLOR;

	if (!myself->isActive || !myself->IsParentActive()) activeColor = ImVec4 PASIVE_COLOR;
	if (myself->focused) activeColor = ImVec4 FOCUSED_COLOR;
	else if (myself->selected) activeColor = ImVec4 SELECTED_COLOR;
	return activeColor;
}
