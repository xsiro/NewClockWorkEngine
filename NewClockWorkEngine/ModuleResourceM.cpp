#include "Globals.h"

#include "ModuleResourceM.h"

ModuleResourceM::ModuleResourceM(bool start_enabled) : Module(start_enabled)
{

}

ModuleResourceM::~ModuleResourceM()
{

}

bool ModuleResourceM::Init()
{
	return true;
}

bool ModuleResourceM::CleanUp()
{
	return true;
}