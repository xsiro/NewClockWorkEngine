#include "Application.h"

Application::Application() : debug(false), renderPrimitives(true), realDT(0.16f)
{
	window = new ModuleWindow();
	input = new ModuleInput();
	renderer3D = new ModuleRenderer3D();
	gui = new ModuleGUI();
	camera = new ModuleCamera3D();
	scene = new ModuleSceneIntroIntro();
	fileSystem = new ModuleFileSystem();
	//resourceManager = new ModuleResourceManager();
	rManager = new ModuleResourceManager();
	audioManager = new ModuleAudioManager();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(fileSystem);
	//AddModule(resourceManager);
	AddModule(input);
	AddModule(scene);
	AddModule(rManager);
	AddModule(audioManager);
	// Renderer last!
	AddModule(renderer3D);
	AddModule(gui);
}

Application::~Application()
{
	for (int i = list_modules.size() - 1; i >= 0; i--)
	{
		if (list_modules[i] != nullptr)
		{
			delete(list_modules[i]);
			list_modules[i] = nullptr;
		}
	}
	list_modules.clear();

}

bool Application::Init()
{
	bool ret = true;

	App = this;

	//time related initializations
	frameCount = 0;
	time = 0;
	timeScale = 1.0f;//TODO will this change from config file in the future if we start with game?
	gameDT = 0.0f;
	realTime = 0;
	realDT = 0.0f;
	gameState = GameStateEnum::STOPPED;//TODO this will change when executing the game
	gameJustStarted = false;

	// Call Init() in all modules
	for (int i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i] != nullptr && ret == true)
			ret = list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("-------------- Application Start --------------");
	for (int i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i] != nullptr && ret == true)
			ret = list_modules[i]->Start();
	}

	ms_timer.Start();

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	Uint32 msDT = ms_timer.Read();
	realTime += msDT;
	realDT = (float)msDT * 0.001f;


	gameStateJustChanged = false;
	GameStateEnum lastGameState = gameState;
	ProcessGameStates(lastRelevantStateChange);
	if (lastGameState != gameState)
	{
		gameStateJustChanged = true;
	}
	lastRelevantStateChange = GameStateEnum::UNKNOWN;

	if (gameState == GameStateEnum::PLAYED || gameState == GameStateEnum::ADVANCEONE)
	{
		frameCount++;//when we finish a frame, add 1 to the frame count
		if ((time + (timeScale * msDT)) < 0)
		{
			time = 0;
			LOG("");
			LOG("[error] Well, well, it seems that someone has tried to go back in time to before the creation of time iself.\nWe have tried that too, and we assure you that is not possible.");
			LOG("[warning] We will now proceed to restore the flow of time as it should be...\nPress 'Pause' button to continue the execution of the game");
			timeScale = 1.0f;
			SetNewGameState(GameStateEnum::PAUSED);
		}
		else
		{
			time += (timeScale * msDT);
		}
		gameDT = (float)msDT * 0.001f * timeScale;
	}
	if (gameState == GameStateEnum::STOPPED && time != 0)
	{
		time = 0;
		frameCount = 0;
		gameDT = 0.0f;
	}

	//FPS buffer for graph display
	fpsBuffer.push_back(1 / realDT);
	while (fpsBuffer.size() >= MAXFPSDISPLAY)
	{
		fpsBuffer.erase(fpsBuffer.begin());
	}
	//Milliseconds buffer for graph display
	millisecondsBuffer.push_back((float)msDT);
	while (millisecondsBuffer.size() >= MAXFPSDISPLAY)
	{
		millisecondsBuffer.erase(millisecondsBuffer.begin());
	}




	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	if (gameJustStarted)
	{
		gameJustStarted = false;
		for (int i = 0; i < list_modules.size(); i++)
		{
			if (list_modules[i] != nullptr && ret == true)
			{
				if (list_modules[i]->GameInit())//TODO consider returning an update_status instead of a bool in the game_init()
					ret = UPDATE_CONTINUE;
				else
					ret = UPDATE_ERROR;
			}
		}
	}




	for (int i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i] != nullptr && ret == true)
			ret = list_modules[i]->PreUpdate(realDT);
	}

	for (int i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i] != nullptr && ret == true)
			ret = list_modules[i]->Update(realDT);
	}

	if (gameState == GameStateEnum::PLAYED || gameState == GameStateEnum::ADVANCEONE)
	{
		for (int i = 0; i < list_modules.size(); i++)
		{
			if (list_modules[i] != nullptr && ret == true)
				ret = list_modules[i]->GameUpdate(gameDT);
		}
	}

	for (int i = 0; i < list_modules.size(); i++)
	{
		if (list_modules[i] != nullptr && ret == true)
			ret = list_modules[i]->PostUpdate(realDT);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = list_modules.size() - 1; i >= 0; i--)
	{
		if (list_modules[i] != nullptr && ret == true)
			ret = list_modules[i]->CleanUp();
	}

	App = nullptr;
	return ret;
}

void Application::ProcessGameStates(GameStateEnum newState)
{

	switch (gameState)
	{
	case GameStateEnum::STOPPED:


		if (newState == GameStateEnum::PLAYED)
		{
			//start play clock
			gameState = GameStateEnum::PLAYED;
			gameJustStarted = true;
		}


		break;
	case GameStateEnum::PLAYED:


		switch (newState)
		{
		case GameStateEnum::STOPPED:
			//reset & stop play clock
			gameState = GameStateEnum::STOPPED;
			break;
		case GameStateEnum::PAUSED:
			//pause play clock
			gameState = GameStateEnum::PAUSED;
			break;
		case GameStateEnum::ADVANCEONE:
			gameState = GameStateEnum::ADVANCEONE;
			break;
		}


		break;
	case GameStateEnum::PAUSED:


		switch (newState)
		{
		case GameStateEnum::STOPPED:
			//reset & stop play clock
			gameState = GameStateEnum::STOPPED;
			break;
		case GameStateEnum::PLAYED:
			//resume play clock
			gameState = GameStateEnum::PLAYED;
			break;
		case GameStateEnum::ADVANCEONE:
			//resume play clock
			gameState = GameStateEnum::ADVANCEONE;
			break;
		}


		break;
	case GameStateEnum::ADVANCEONE:


		//do smth
		gameState = GameStateEnum::PAUSED;


		break;
	}


}

GameStateEnum Application::GetGameState() const
{
	return gameState;
}

void Application::SetNewGameState(GameStateEnum newState)
{
	if (lastRelevantStateChange != newState)
	{
		lastRelevantStateChange = newState;
	}
}

Uint32 Application::GetFrameCount() const
{
	return frameCount;
}

float Application::GetTime() const
{
	return (time * 0.001f);
}

float Application::GetTimeScale() const
{
	return timeScale;
}

float Application::GetGameDT() const
{
	return gameDT;
}

float Application::GetRealTime() const
{
	return (realTime * 0.001f);
}

float Application::GetRealDT() const
{
	return realDT;
}

void Application::SetNewTimeScale(float newTimeScale)
{
	const float maxMinTimeScale = 5.0f;//this changes the max min time scale allowed Note:: don't enter a negative value

	timeScale = max(newTimeScale, -maxMinTimeScale);
	timeScale = min(timeScale, maxMinTimeScale);
}

bool Application::HasGameStateChanged() const
{
	return gameStateJustChanged;
}

bool Application::HasGameStateChanged(GameStateEnum& currentGameState)
{
	if (gameStateJustChanged)
	{
		currentGameState = gameState;
	}

	return gameStateJustChanged;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

Application* App = nullptr;
