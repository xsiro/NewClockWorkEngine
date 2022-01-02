#include "Application.h"
#include "Window.h"

#include "ModuleGui.h"

#include "Win_Play.h"

#include "imgui.h"

//#include "Dependecies/mmgr/mmgr.h"

Win_Play::Win_Play(bool _active) : Window(_active)
{

}

Win_Play::~Win_Play()
{

}

void Win_Play::Init()
{

}

void Win_Play::Draw()
{
	if (!active)
		return;

	if (!ImGui::Begin("Play", &active))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Play"))
	{
		App->Play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		App->Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		App->Stop();
	}
	ImGui::SameLine();

	ImGui::Text("Play Time: %f", App->GetPlayTime());

	ImGui::SameLine();

	float timeMultiplier = App->GetTimeMultiplier();
	if (ImGui::DragFloat("Time Multiplier", &timeMultiplier, 0.1, 0.1, 5))
	{
		App->SetTimeMultiplier(timeMultiplier);
	}

	if (ImGui::IsWindowHovered())
		App->gui->mouseHovered = true;
	ImGui::End();
}

void Win_Play::CleanUp()
{

}