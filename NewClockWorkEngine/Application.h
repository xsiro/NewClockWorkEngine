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
#include "Config.h"



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
	Timer	frame_time;

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
	int GameMaxFPS = 60;
	void ToSave();
	void Play();
	void Pause();
	void Stop();

	inline void SetTimeMultiplier(float _timeMultiplier) { timeMultiplier = _timeMultiplier; };

	inline float GetPlayTime()const { return playTime; };
	inline float GetTimeMultiplier()const { return timeMultiplier; };

	inline bool IsInPlayMode()const { return play; };

	inline float GetDt()const { return dt; }
	inline float GetPlayDt()const { return playDt; };

private:


	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
	void Save();

public:
	int	miliseconds;
	int contFPS;
	int	last_ms;
	int	last_fps;
	float max_ms;
	Uint32 frames;
	bool closewindow;
	bool debug;
	bool toSave;
	float	playDt = 0;	//This dt effects objects in Play mode
	float	dt = 0;		//independent App dt
	float	frameCap = 0;
	int		frameCount = 0;

private:
	bool play = false;
	bool paused = false;
	float playTime = 0;
	float timeMultiplier = 1;
};
extern Application* App;
extern std::vector<std::string> log_record;