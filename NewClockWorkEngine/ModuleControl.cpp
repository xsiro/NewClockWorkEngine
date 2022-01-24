#include "ModuleControl.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"
#include "ModuleTransform.h"
#include "ModuleInput.h"

ModuleControl::ModuleControl(GameObject* owner, unsigned int ID) :Component(ComponentType::CONTROL, owner, ID), speed(1)
{}

ModuleControl::~ModuleControl()
{
}

void ModuleControl::OnEditor()
{

	bool activeAux = active;

	std::string headerName = "Control";
	std::string suffixLabel = "##Control";
	std::string actualname;
	suffixLabel += std::to_string(ID);
	if (!activeAux)headerName += " (not active)";

	ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

	if (!activeAux)ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

	if (ImGui::CollapsingHeader(headerName.c_str(), headerFlags))
	{
		if (!activeAux)ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 0.8f));
		ImGui::Spacing();

		actualname = "IS ACTIVE" + suffixLabel + "Checkbox";
		ImGui::Checkbox(actualname.c_str(), &active);

		ImGui::Separator();
		ImGui::Indent();
		ImGui::Spacing();
		ImGui::Spacing();
		//actual Component code here

		actualname = "Speed" + suffixLabel;
		if (ImGui::SliderFloat(actualname.c_str(), &speed, 0.0f, 10.0f))
		{
			if (speed < 0) speed = 0;
		}

		ImGui::Separator();
		ImGui::Unindent();

		actualname = "Delete Control Component" + suffixLabel;
		if (ImGui::BeginPopup(actualname.c_str(), ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("you are about to delete\n this component");
			actualname = "Go ahead" + suffixLabel;
			if (ImGui::Button(actualname.c_str()))
			{
				toDelete = true;
			}

			ImGui::SameLine();
			actualname = "Cancel" + suffixLabel;
			if (ImGui::Button(actualname.c_str()))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		float maxWidth = ImGui::GetWindowContentRegionMax().x;
		ImGui::SetCursorPosX(maxWidth - 50);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.25f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		actualname = "Delete" + suffixLabel + "component";
		if (ImGui::Button(actualname.c_str()))
		{
			actualname = "Delete Control Component" + suffixLabel;
			ImGui::OpenPopup(actualname.c_str());
		}

		if (!activeAux)ImGui::PopStyleColor();
		ImGui::PopStyleColor(2);
	}
	if (!activeAux)ImGui::PopStyleColor();
}

bool ModuleControl::GameUpdate(float gameDT)
{
	if (this->IsActive())
	{
		ManageMovement(gameDT);
	}
	return true;
}

bool ModuleControl::GameInit()
{
	return true;
}

bool ModuleControl::ManageMovement(float dt)
{
	bool ret = false;
	float3 auxPos(0.0f, 0.0f, 0.0f);
	float auxSpeed = speed * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		auxSpeed *= 3;
	}

	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
	{
		auxPos.z -= auxSpeed;
		ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
	{
		auxPos.z += auxSpeed;
		ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
	{
		auxPos.x += auxSpeed;
		ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
	{
		auxPos.x -= auxSpeed;
		ret = true;
	}

	if (ret == true) {
		auxPos = owner->GetComponent<ModuleTransform>()->GetGlobalPosition() + auxPos;
		owner->GetComponent<ModuleTransform>()->SetGlobalPosition(auxPos);
	}

	return ret;
}

bool ModuleControl::Update(float dt)
{
	return true;
}

float ModuleControl::GetSpeed() const
{
	return speed;
}

void ModuleControl::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}
