#pragma once

#include <vector>
#include <string>
#include "Glew/include/glew.h"

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ModuleImporter.h"
#include "FileSystem.h"
#include "ModuleResourceM.h"
#include "Brofiler/Brofiler.h"




class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	FileSystem* filesystem;
	ModuleResourceM* resourcemanager;

private:

	Timer	ms_timer;
	Timer	fps_timer;
	
	std::vector<Module*> list_modules;

public:

	Application();
	~Application();

	void RequestBrowser(const char* url)const;
	int CPUCount();
	int CPUCache();
	int SystemRAM();
	const char* SystemCaps();
	const char* Brand();
	const char* Model();
	int Budget();
	int Usage();
	int Available();
	int Reserved();
	std::string Caps;
	bool Init();
	update_status Update();
	bool CleanUp();
	uint GetFRLimit() const;
	void ExitApp();
	void SetFRLimit(uint max_framerate);
	
	

private:


	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

public:
	int	miliseconds;
	int contFPS;
	int	last_ms;
	int	last_fps;
	float max_ms;
	float dt;
	Uint32 frames;
	bool closewindow;
	bool debug;

	
};
	extern Application* App;
	extern std::vector<std::string> log_record;