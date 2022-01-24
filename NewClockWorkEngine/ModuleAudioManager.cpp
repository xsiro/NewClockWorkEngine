#include "ModuleAudioManager.h"
#include "wwise_libraries.h"
#include "Wwise/low_level_IO/Win32/AkFilePackageLowLevelIOBlocking.h"


//Testing Includes:
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"
#include "ModuleReverbZone.h"

//PlugIn
//#include "Wwise/AK/Plugin/AkRoomVerbFXFactory.h" //room reverb

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

ModuleAudioManager::ModuleAudioManager(bool start_enabled) :Module(start_enabled)
{
	activeListener = nullptr;

}

ModuleAudioManager::~ModuleAudioManager()
{
	activeListener = nullptr;
}

bool ModuleAudioManager::Init()
{
	//Initialize Memory Manager
	AkMemSettings memSettings;

	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{

		LOG("[error] Could not create the memory manager.");

		return false;

	}

	//Initialize Stream Manager

	AkStreamMgrSettings stmSettings;

	AK::StreamMgr::GetDefaultSettings(stmSettings);



	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(stmSettings))
	{

		LOG("[error] Could not create the Streaming Manager");

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
		LOG("[error] Could not create the streaming device and Low-Level I/O system");
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
		LOG("[error] Could not initialize the Sound Engine.");
		return false;
	}

	// Initialize the music engine
	// Using default initialization parameters

	AkMusicSettings musicInit;

	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		LOG("[error] Could not initialize the Music Engine.");
		return false;
	}

	//Initialize Spatial Audio
	// Using default initialization parameters

	AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 

	if (AK::SpatialAudio::Init(settings) != AK_Success) //TODO settings was passed as a reference but triggered an error
	{
		LOG("[error] Could not initialize the Spatial Audio.");
		return false;
	}

	//Initialize Communications
#ifndef AK_OPTIMIZED
// Initialize communications (not in release build!)
	AkCommSettings commSettings;

	AK::Comm::GetDefaultInitSettings(commSettings);

	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		LOG("[error] Could not initialize communication.");
		return false;
	}

#endif // AK_OPTIMIZED



	//=======================================================================================================================
	//LOAD SOUNDBANKS
	// Setup banks path	
#ifndef AK_OPTIMIZED
	g_lowLevelIO.SetBasePath(AKTEXT("Assets/wwise/PhoebusWwise/GeneratedSoundBanks/Windows/"));
#else
	g_lowLevelIO.SetBasePath(AKTEXT("Assets/wwise/"));
#endif // AK_OPTIMIZED

	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));
	// Load banks synchronously (from file name).

	AkBankID bankID; // Not used. These banks can be unloaded with their file name.

	AKRESULT eResult = AK::SoundEngine::LoadBank("Init.bnk", bankID);
	if (eResult == AK_Success)
	{
		LOG("Bank 'Init.bnk' has been loaded successfully");
	}
	else
	{
		LOG("[error] Loading 'Init.bnk'");
	}



	eResult = AK::SoundEngine::LoadBank("Main.bnk", bankID);
	if (eResult == AK_Success)
	{
		LOG("Bank 'Main.bnk' has been loaded successfully");
	}
	else
	{
		LOG("[error] Loading 'Main.bnk'");
	}

	return true;
}

bool ModuleAudioManager::GameInit()
{

	UpdateListener();

	return true;
}

bool ModuleAudioManager::Start()
{
	return true;
}

update_status ModuleAudioManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleAudioManager::PostUpdate(float dt)
{
	GameStateEnum currentState = GameStateEnum::UNKNOWN;
	if (App->HasGameStateChanged(currentState))
	{
		switch (currentState)
		{
		case GameStateEnum::STOPPED:
			StopAllSounds();
			break;
		case GameStateEnum::PLAYED:
			ResumeAllSounds();
			break;
		case GameStateEnum::PAUSED:
			PauseAllSounds();
			break;
		case GameStateEnum::ADVANCEONE:
			ResumeAllSounds();//TODO first it advances one then it pauses ???
			break;
		}
	}



	// Process bank requests, events, positions, RTPC, etc.
	AK::SoundEngine::RenderAudio();

	return UPDATE_CONTINUE;
}

update_status ModuleAudioManager::GameUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleAudioManager::UpdateListener()
{
	if (enabled)//TODO bad code structure makes us need this enabled in lots of audio manager methods. Fix it
	{

		AkGameObjectID id = 0;
		//TODO trashy way to do it, we do not want to set the listener every frame
		if (activeListener != nullptr)
		{
			id = activeListener->ID;
		}
		else
		{
			LOG("[warning] There is no audio listener component active. Sounds won't be heard");
		}

		// Set one listener as the default.
		AK::SoundEngine::SetDefaultListeners(&id, 1);
	}
}

bool ModuleAudioManager::CleanUp()
{

	AK::SoundEngine::UnregisterAllGameObj();

	AK::SoundEngine::ClearBanks();

	// Terminate Communication Services
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
	enabled = false;
	return true;
}

void ModuleAudioManager::RegisterNewAudioObj(unsigned int componentID)
{
	if (enabled)
		AK::SoundEngine::RegisterGameObj(componentID);
}

void ModuleAudioManager::UnRegisterAudioObj(unsigned int componentID)
{
	if (enabled)
		AK::SoundEngine::UnregisterGameObj(componentID);
}

void ModuleAudioManager::UnRegisterAllAudioObjs() const
{
	if (activeListener != nullptr)
	{
		activeListener->SetAsListener(false);
	}
	AK::SoundEngine::UnregisterAllGameObj();
}

void ModuleAudioManager::SetAudioObjTransform(unsigned int componentID, float4x4 transform)
{
	float3 pos;
	float3 scale;
	float3x3 rot;
	float3 front(0.0f, 0.0f, 1.0f);
	float3 up(0.0f, 1.0f, 0.0f);

	transform.Decompose(pos, rot, scale);
	//TODO take orientation too?
	up = up * rot;
	front = front * rot;

	AkSoundPosition newPos;
	newPos.SetPosition(pos.x, pos.y, pos.z);
	newPos.SetOrientation(front.x, front.y, front.z, up.x, up.y, up.z);

	AK::SoundEngine::SetPosition(componentID, newPos);
}

void ModuleAudioManager::SendAudioObjEvent(unsigned int componentID, std::string eventName)const
{
	if (enabled)
		AK::SoundEngine::PostEvent(eventName.c_str(), componentID); //plays sound
}

void ModuleAudioManager::SendStopEvent(unsigned int componentID, std::string eventName)const
{
	if (enabled)
		AK::SoundEngine::ExecuteActionOnEvent(eventName.c_str(), AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Stop, componentID);
}

void ModuleAudioManager::ChangeRTPCValue(unsigned int componentID, std::string RPTCname, float value)
{
	if (enabled)
		AK::SoundEngine::SetRTPCValue(RPTCname.c_str(), value, componentID);
}


void ModuleAudioManager::StopAllSounds() const
{
	if (enabled)
		AK::SoundEngine::StopAll();
}

void ModuleAudioManager::PauseAllSounds() const
{
	if (enabled)
		AK::SoundEngine::PostEvent("PauseAll", AK_INVALID_GAME_OBJECT); //Invalid Game Object also means all game objects
}

//Note that this is not a master resume, that means that will only remove 1 pause action if many
void ModuleAudioManager::ResumeAllSounds() const
{
	if (enabled)
		AK::SoundEngine::PostEvent("ResumeAll", AK_INVALID_GAME_OBJECT); //Invalid Game Object also means all game objects
}

void ModuleAudioManager::PauseObjSounds(unsigned int componentID)
{
	if (enabled)
		AK::SoundEngine::PostEvent("PauseObj", componentID);
}

//Note that this is not a master resume, that means that will only remove 1 pause action if many
void ModuleAudioManager::ResumeObjSounds(unsigned int componentID)
{
	if (enabled)
		AK::SoundEngine::PostEvent("ResumeObj", componentID);
}

void ModuleAudioManager::StopObjSounds(unsigned int componentID)
{
	if (enabled)
		AK::SoundEngine::PostEvent("StopObj", componentID);
}

void ModuleAudioManager::AddRevZone(ModuleReverbZone* revZone)
{
	revAreas.push_back(revZone);
}

bool ModuleAudioManager::RemoveRevZone(ModuleReverbZone* revZone)
{
	bool ret = false;
	for (int i = 0; i < revAreas.size(); i++)
	{
		if (revAreas[i] == revZone)
		{
			revAreas.erase((revAreas.begin() + i));
			ret = true;
			break;
		}
	}
	return ret;
}

void ModuleAudioManager::AuxSendValues(AkReal32 value, const char* targetBus, unsigned long listenerID, unsigned long sourceID) const
{
	AkAuxSendValue data;
	data.listenerID = listenerID;
	data.auxBusID = AK::SoundEngine::GetIDFromString(targetBus);
	data.fControlValue = value;

	AKRESULT res = AK::SoundEngine::SetGameObjectAuxSendValues(sourceID, &data, 1);
}
