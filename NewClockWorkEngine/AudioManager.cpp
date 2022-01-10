#include "AudioManager.h"
#include <assert.h>
#include "wwise_libraries.h"
#include "Wwise/low_level_IO/Win32/AkFilePackageLowLevelIOBlocking.h"

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

AudioManager::AudioManager(bool start_enabled) :Module(start_enabled)
{
}

AudioManager::~AudioManager()
{
}

bool AudioManager::Init()
{
	/*AkMemSettings memSettings;

	memSettings.uMaxNumPools = 20;

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)

	{
		LOG("[error]Could not create the memory manager.");

		return false;

	}*/
	

	return true;
}

update_status AudioManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool AudioManager::CleanUp()
{
	return true;
}
