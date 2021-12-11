#pragma once
#include "Module.h"
#include "SDL.h"

class Application;

class ModuleResourceM : public Module
{
public:

	ModuleResourceM(bool start_enabled = true);

	// Destructor
	virtual ~ModuleResourceM();

	bool Init();
	bool CleanUp();



public:

};

