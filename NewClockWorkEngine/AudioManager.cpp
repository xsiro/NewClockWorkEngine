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
	AkMemSettings memSettings;

	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert("[error]Could not create the memory manager.");

		return false;

	}
	
	AkStreamMgrSettings stmSettings;

	AK::StreamMgr::GetDefaultSettings(stmSettings);



	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(stmSettings))
	{

		assert(!"Could not create the Streaming Manager");

		return false;

	}


	// Create a streaming device with blocking low-level I/O handshaking.
	// Note that you can override the default low-level I/O module with your own. 

	AkDeviceSettings deviceSettings;

	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);


	// Customize the streaming device settings here.
	// CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
	// in the Stream Manager, and registers itself as the File Location Resolver.

	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;

	}

	//Initialize Sound Engine
	// Create the Sound Engine
	// Using default initialization parameters

	AkInitSettings initSettings;

	AkPlatformInitSettings platformInitSettings;

	AK::SoundEngine::GetDefaultInitSettings(initSettings);

	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);


	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}

	// Initialize the music engine
	// Using default initialization parameters

	AkMusicSettings musicInit;

	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}

	//Initialize Spatial Audio
	// Using default initialization parameters

	AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 

	if (AK::SpatialAudio::Init(settings) != AK_Success) //TODO settings was passed as a reference but triggered an error
	{
		assert(!"Could not initialize the Spatial Audio.");
		return false;
	}

	//Initialize Communications
#ifndef AK_OPTIMIZED
// Initialize communications (not in release build!)
	AkCommSettings commSettings;

	AK::Comm::GetDefaultInitSettings(commSettings);

	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		return false;
	}

#endif // AK_OPTIMIZED

	return true;
}

update_status AudioManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool AudioManager::CleanUp()
{
#ifndef AK_OPTIMIZED
	AK::Comm::Term();

#endif // AK_OPTIMIZED

	// Terminate Spatial Audio
	//AK::SpatialAudio::Term();//TODO this method doesn't exist ???

	 // Terminate the music engine
	AK::MusicEngine::Term();

	// Terminate the sound engine
	AK::SoundEngine::Term();


	// Terminate the streaming device and streaming manager
   //CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device 
   // that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
	g_lowLevelIO.Term();

	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager
	AK::MemoryMgr::Term();

	return true;
}
