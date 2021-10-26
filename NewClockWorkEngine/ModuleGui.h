#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"

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
	bool Start();

	// Called before all Updates
	update_status PreUpdate(float dt);

	// Called every frame
	update_status Update(float dt);

	// Called after all Updates
	update_status PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();
	
public:
	int fps;
	int height;
	int width;
	bool check = true;
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fulldesktop;
	bool vertexlines;
	bool facelines;
	float brightness;
	bool scroll;
	//our state
	bool show_demo_window;
	bool mainwindow;
	bool show_config;
	bool show_console;
	bool about_window;
	bool* dockingwindow;
	const char* GetName() const;
	const char* name;
	void ClearLog();
	void ConsoleLog(char* logStr);
	void ConsoleOutput();
	update_status Dock(bool* p_open);

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
