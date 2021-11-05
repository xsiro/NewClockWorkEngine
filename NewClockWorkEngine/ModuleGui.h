#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"

#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_video.h"

#include <list>
#include <string>
#include <vector>

#define THRESHOLD_LOGS 1000



class ModuleGui : public Module
{
public:

	ModuleGui(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleGui();

	// Called when before render is available
	bool Awake();

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
	
public:

	int fps;
	int height;
	int width;

	float brightness;

	bool check=false;
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fulldesktop;
	bool vertexlines;
	bool facelines;
	bool scroll;
	bool show_demo_window;
	bool mainwindow;
	bool show_config;
	bool show_console;
	bool about_window;
	bool inspector;
	bool* dockingwindow;
	bool hierarchy;
	bool depthtest;
	bool cullface;
	bool lighting;
	bool polygonssmooth;
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


	const char* GetName() const;
	const char* name;

	void ClearLog();
	void ConsoleLog(char* logStr);
	void ConsoleOutput();
	void Hierarchy();
	void GameObjectsHierarchy();
	void Draw();
	

	SDL_GLContext gl_context;

	ImVec4 clear_color;
	ImVec2 mouseScenePosition;
	ImVec2 image_size;
	std::vector<char*> console_log;


private:

	std::vector<float> fps_log;
	std::vector<float> ms_log;
	std::vector<char*> logs;
};

#endif // __ModuleGui_H__
