#include "Win_Console.h"
#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_opengl3.h"
#include "imgui/include/imgui_impl_sdl.h"
#include "Application.h"
#include "Globals.h"

Win_Console::Win_Console(bool _active) : Window(_active)
{

}

Win_Console::~Win_Console()
{}

void Win_Console::Init()
{

}

void Win_Console::Draw()
{
	if (!active)
		return;
	if (!ImGui::Begin("Console", &active))
	{
			ImGui::End();
			return;
	}
	if (ImGui::Button("Delete"))
	{
		CleanUp();
	}
	for (uint i = 0; i < logs.size(); ++i)
	{
		ImGui::TextUnformatted(logs[i]);
	}
	if (ImGui::IsWindowHovered())
		App->gui->mouseHovered = true;
	ImGui::End();
}

void Win_Console::CleanUp()
{
	for (int i = 0; i < logs.size(); i++)
		free(logs[i]);
	logs.clear();
}

void Win_Console::ConsoleLog(char* logStr)
{
	if (logs.size() + 1 > THRESHOLD_LOGS)
	{
		ClearLog();
	}

	char* tmp = _strdup(logStr);

	logs.push_back(tmp);
}

void Win_Console::ClearLog()
{
	for (int i = 0; i < logs.size(); ++i)
	{
		free(logs[i]);
	}
	logs.clear();
}
