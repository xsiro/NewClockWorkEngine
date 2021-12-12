#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui/include/imgui.h"

#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_video.h"

#include <list>
#include <string>
#include <vector>

class Window;
class Win_Inspector;
class Win_About;
class Win_Console;
class Win_Configuration;
class Win_Hierarchy;

class ModuleGui : public Module
{
public:

	ModuleGui( bool start_enabled = true);

	ModuleGui::~ModuleGui();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();


	void Draw();
	void AddWindow(Window* window);
	void Log(char* text);
	void LogFPS(float fps, float ms);
	
	update_status Dock(bool* p_open);

	bool GetVL();
	bool GetFL();
	bool GetCheck();
	bool GetWireframe();
	bool Save(ConfigNode* config) override;
	bool IsMouseHovering();

public:

	bool scroll;
	bool show_demo_window;
	bool mainwindow;
	bool* dockingwindow;
	bool texture2D;
	bool checker;
	bool cube;
	bool pyramid;
	bool cylinder;
	bool sphere;
	bool showmaterial;
	bool mouseHovered = false;

	ImVec4 clear_color;
	ImVec2 mouseScenePosition;
	ImVec2 image_size;

	std::vector<Window*> winArray;
	Win_About* about = nullptr;
	Win_Inspector* inspector = nullptr;
	Win_Console* console = nullptr;
	Win_Hierarchy* hierarchy = nullptr;
	Win_Configuration* config = nullptr;

private:

	GameObject* object = nullptr;

	std::vector<float> fps_log;
	std::vector<float> ms_log;
};


