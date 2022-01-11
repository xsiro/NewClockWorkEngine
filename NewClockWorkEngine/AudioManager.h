#pragma once
#include "Module.h"
#include "Globals.h"

class AudioManager :public Module
{
public:
	AudioManager(bool start_enabled = true);
	~AudioManager();

	bool Init();

	update_status Update(float dt) override;
	bool CleanUp();


};

