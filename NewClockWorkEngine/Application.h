#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleSceneIntro.h"
#include "ModuleImporter.h"
#include "Glew/include/glew.h"
#include "FileSystem.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleImporter* importer;
	FileSystem* filesystem;

private:

	Timer	ms_timer;
	
	p2List<Module*> list_modules;

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