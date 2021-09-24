#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include "ImGui/include/imgui.h"

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

	//our state
	bool show_demo_window;
	bool mainwindow;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


private:

	SDL_GLContext gl_context;
	ImGuiIO* io = nullptr;
	

};

#endif // __ModuleGui_H__
