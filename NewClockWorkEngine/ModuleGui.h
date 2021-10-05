#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"

#include <list>
#include <string>
#include <vector>

struct log_message 
{
	std::string log_text;
	int warning_level;
};

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
	void AddConsoleLog(const char* log, int warning_level);
	void ConsoleMenu();
	bool DrawConsole(ImGuiIO& io);

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
	bool show_console=true;
	const char* GetName() const;
	const char* name;
	void ClearLog();
	void ConsoleOutput();

	ImVec4 clear_color;
	ImVec2 mouseScenePosition;
	ImVec2 image_size;
	std::vector<log_message> console_log;

private:

	std::vector<float> fps_log;
	std::vector<float> ms_log;
	std::vector<char*> logs;
};

#endif // __ModuleGui_H__
