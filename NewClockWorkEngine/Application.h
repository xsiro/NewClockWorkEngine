#pragma once

#include <vector>
#include <string>

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
#include "Glew/include/glew.h"



class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;

	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleImporter* importer;


private:

	Timer	ms_timer;
	
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
	bool closewindow;
	
private:


	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

public:
	float max_ms;
	float fps;
	float dt;

	
};
	extern Application* App;
	extern std::vector<std::string> log_record;