#include "AudioManager.h"
#include <assert.h>
#include "wwise_libraries.h"
#include "Wwise/low_level_IO/Win32/AkFilePackageLowLevelIOBlocking.h"
#include "Application.h"
#include "ModuleGui.h"
#include "GameObject.h"

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

AudioManager::AudioManager(bool start_enabled) :Module(start_enabled)
{
	activeListener = nullptr;
}

AudioManager::~AudioManager()
{
	activeListener = nullptr;
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
	g_lowLevelIO.SetBasePath(AKTEXT("Assets/wwise/PhoebusWwise/WwiseProject/Windows/"));
	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));
	// Load banks synchronously (from file name).

	AkBankID bankID; // Not used. These banks can be unloaded with their file name.

	AKRESULT eResult = AK::SoundEngine::LoadBank("Init.bnk", bankID);

	//assert(eResult != AK_Success);

	eResult = AK::SoundEngine::LoadBank("Main.bnk", bankID);
	return true;
}

bool AudioManager::Start()
{
	AkGameObjectID id = App->scene_intro->rootObject->ID;

	// Register the main listener. //TODO this will change in the near future, also for now the main listener is also the player
	AK::SoundEngine::RegisterGameObj(id);
	// Set one listener as the default.
	AK::SoundEngine::SetDefaultListeners(&id, 1);

	AK::SoundEngine::PostEvent("destroyed_turret", App->scene_intro->rootObject->ID);
	return true;
}

update_status AudioManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status AudioManager::GameUpdate(float dt)
{
	// Process bank requests, events, positions, RTPC, etc.
	AK::SoundEngine::RenderAudio();

	return UPDATE_CONTINUE;
}

bool AudioManager::CleanUp()
{
	AK::SoundEngine::UnregisterAllGameObj();

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
