#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"

#include <list>
#include <string>
#include <vector>

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
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fulldesktop;
	bool vertexlines;
	bool facelines;
	float brightness;
	//our state
	bool show_demo_window;
	bool mainwindow;
	bool show_config;
	ImVec4 clear_color;
	ImVec2 mouseScenePosition;
	ImVec2 image_size;

private:

	std::vector<float> fps_log;
	std::vector<float> ms_log;
};

#endif // __ModuleGui_H__
