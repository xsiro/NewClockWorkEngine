#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleGUI.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleAudioManager.h"

enum class GameStateEnum
{
	STOPPED,
	PLAYED,
	PAUSED,
	ADVANCEONE,
	UNKNOWN
};

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleGUI* gui;
	ModuleCamera3D* camera;
	ModuleSceneIntroIntro* scene;
	ModuleFileSystem* fileSystem;
	//ModuleResourceManager* resourceManager;
	ModuleResourceManager* rManager;
	ModuleAudioManager* audioManager;

	bool debug;
	bool renderPrimitives;
	std::vector<float> fpsBuffer;
	std::vector<float> millisecondsBuffer;

private:

	std::vector<Module*> list_modules;


	//clock related
	GameStateEnum gameState;
	GameStateEnum lastRelevantStateChange;
	Timer	ms_timer;

	//Game clock
	Uint32 frameCount; //app graphics frames since game start
	Uint32 time;//ms since game start (Game Clock)
	float timeScale; //scale at which time is passing(Game Clock)
	float gameDT; //last frame time expressed in seconds (Game Clock)
	//Real Clock
	Uint32 realTime;//ms since engine start(Real Time Clock)
	float realDT; //last frame time expressed in seconds(Real Time Clock)

	bool gameJustStarted;
	bool gameStateJustChanged;//true only during the frame when the game state changes

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();



	//Time & Game related==========================================

	GameStateEnum GetGameState()const;
	void SetNewGameState(GameStateEnum newState);
	//Clock Related getters

	//returns app graphics frames since game start
	Uint32 GetFrameCount()const;
	//returns the amount of second since game start (Game Clock)
	float GetTime()const;
	//returns scale at which time is passing(Game Clock)
	float GetTimeScale()const;
	//returns the last frame time expressed in seconds (Game Clock)
	float GetGameDT()const;
	//returns the amount of seconds since engine start(Real Time Clock)
	float GetRealTime()const;
	//returns the last frame time expressed in seconds(Real Time Clock)
	float GetRealDT()const;
	
	//Clock Related Setters
	void SetNewTimeScale(float newTimeScale);

	//============================================================

	//returns true if the game state has changed in the last frame
	bool HasGameStateChanged()const;
	//returns true if the game state has changed in the last frame, it also returns the current Game state if true
	bool HasGameStateChanged(GameStateEnum& currentGameState);

private:

	void ProcessGameStates(GameStateEnum newState);

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};