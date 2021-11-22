#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include "imgui/include/imgui.h"

#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_video.h"

#include <list>
#include <string>
#include <vector>

class Window;
class Win_Console;
class Win_Configuration;

class ModuleGui : public Module
{
public:

	ModuleGui(Application* app, bool start_enabled = true);

	// Destructor
	ModuleGui::~ModuleGui();

	// Call before first frame
	bool Init();

	// Called before all Updates
	update_status PreUpdate(float dt);

	// Called every frame
	update_status Update(float dt);

	// Called after all Updates
	update_status PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	//Docking
	update_status Dock(bool* p_open);

	void AddWindow(Window* window);
	void Log(char* text);
	void LogFPS(float fps, float ms);
	
public:

	bool check=false;
	bool vertexlines;
	bool facelines;
	bool scroll;
	bool show_demo_window;
	bool mainwindow;
	bool show_config;
	bool about_window;
	bool inspector;
	bool* dockingwindow;
	bool hierarchy;
	bool depthtest;
	bool cullface;
	bool lighting;
	bool polygonssmooth;
	bool colormaterial;
	bool material;
	bool texture2D;
	bool cubemap;
	bool checker;
	bool wireframe;
	bool cube;
	bool pyramid;
	bool cylinder;
	bool sphere;
	bool showmaterial;

	const char* name;

	
	void GameObjectsHierarchy();
	void Draw();
	

	SDL_GLContext gl_context;

	ImVec4 clear_color;
	ImVec2 mouseScenePosition;
	ImVec2 image_size;

	std::vector<Window*> winArray;
	Win_Console* console = nullptr;
	Win_Configuration* config = nullptr;


private:

	GameObject* object = nullptr;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
};

#endif // __ModuleGui_H__
