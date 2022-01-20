#pragma once
#include "Module.h"
#include "Globals.h"
#include "AudioListener.h"
class AudioManager :public Module
{
public:
	AudioManager(bool start_enabled = true);
	~AudioManager();

	bool Init();
	bool Start();
	update_status Update(float dt) override;
	update_status GameUpdate(float dt) override;
	bool CleanUp();

public:

	AudioListener* activeListener;
};

