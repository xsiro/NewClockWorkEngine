#include "AudioSource.h"
#include "imgui/imgui.h" //On Editor usage. TODO: cant this be done in another way to not have this here?
#include "imgui/imgui_internal.h"
#include <string>
#include "Application.h"
#include "ModuleAudioManager.h"
#include "GameObject.h"
#include "ModuleTransform.h"
#include "AudioEvent.h"
#include "ModuleReverbZone.h"
#include "AK/SoundEngine/Common/AkTypes.h"

AudioSource::AudioSource(GameObject* owner, unsigned int ID) :Component(ComponentType::AUDIO_SOURCE, owner, ID), volume(50.0f),
musicChangeTime(30.0f), musicTimeCounter(0.0f), userPitch(1.0f)
{
	App->audioManager->RegisterNewAudioObj(this->ID);
}

AudioSource::~AudioSource()
{
	DeleteAllAudioEvents();

	if (App != nullptr && App->audioManager != nullptr)
	{
		App->audioManager->UnRegisterAudioObj(this->ID);
	}
}

void AudioSource::OnEditor()
{
	bool activeAux = active;

	std::string headerName = "AudioSource";
	std::string suffixLabel = "##AudioSource";
	std::string actualname;
	suffixLabel += std::to_string(ID);
	if (!activeAux)headerName += " (not active)";

	ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

	if (!activeAux)ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

	actualname = headerName + suffixLabel;
	if (ImGui::CollapsingHeader(actualname.c_str(), headerFlags))
	{
		if (!activeAux)ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 0.8f));
		ImGui::Spacing();

		GameStateEnum currentGameState = App->GetGameState();

		actualname = "IS ACTIVE" + suffixLabel + "Checkbox";
		if (ImGui::Checkbox(actualname.c_str(), &active))
		{
			if (!active)
			{
				//stop sound if offline pause sound if online

				if (currentGameState == GameStateEnum::STOPPED)
				{
					for (int i = 0; i < events.size(); i++)
					{
						//stop curr object sounds here
						App->audioManager->StopObjSounds(this->ID);
					}
				}
				else if (currentGameState != GameStateEnum::UNKNOWN)
				{
					for (int i = 0; i < events.size(); i++)
					{
						//pause curr object sounds here
						App->audioManager->PauseObjSounds(this->ID);
					}
				}
			}
			else if (currentGameState != GameStateEnum::STOPPED && currentGameState != GameStateEnum::UNKNOWN)
			{
				for (int i = 0; i < events.size(); i++)
				{
					//resume playing (only in game)
					App->audioManager->ResumeObjSounds(this->ID);

				}
			}
		}

		ImGui::Separator();
		ImGui::Indent();
		ImGui::Spacing();
		ImGui::Spacing();

		//actual Component code here

		bool disableButtons = true;
		if (currentGameState == GameStateEnum::STOPPED)//makes it so the audio events can only be edited when in offline mode
		{
			disableButtons = false;
		}
		if (disableButtons)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.5f, 1.0f, 1.0f));
		}

		actualname = "New event name";
		ImGui::InputTextWithHint(suffixLabel.c_str(), actualname.c_str(), newEvName, IM_ARRAYSIZE(newEvName), ImGuiInputTextFlags_CharsNoBlank);
		actualname = "Create SFX" + suffixLabel;
		if (ImGui::Button(actualname.c_str()))
		{
			std::string evName = newEvName;
			if (!evName.empty())
				CreateAudioEvent(evName, AudioEventType::SFX);
			else
				LOG("[warning] Please enter a valid name");
		}
		ImGui::SameLine();
		actualname = "Create MusicBlend" + suffixLabel;
		if (ImGui::Button(actualname.c_str()))
		{
			std::string evName = newEvName;
			if (!evName.empty())
				CreateAudioEvent(evName, AudioEventType::MUSIC);
			else
				LOG("[warning] Please enter a valid name");
		}
		ImGui::Spacing();
		ImGui::Separator();


		for (int i = 0; i < events.size(); i++)
		{
			ImGui::Spacing();
			ImGui::Spacing();
			//Print all events here
			ImGui::Columns(2, 0, false);
			float maxWidth = ImGui::GetWindowContentRegionMax().x;
			ImGui::SetColumnWidth(-1, maxWidth - 125);
			actualname = "Event:  " + events[i]->GetEventName();
			if (ImGui::CollapsingHeader(actualname.c_str(), ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth))
			{
				if (events[i]->GetType() == AudioEventType::SFX)
				{
					actualname = "Type: Sound Effect";
					ImGui::Text(actualname.c_str());
					actualname = "Delete Event" + suffixLabel + events[i]->GetEventName();
					if (ImGui::Button(actualname.c_str()))
					{
						events[i]->toDelete = true;
					}
				}
				else if (events[i]->GetType() == AudioEventType::MUSIC)
				{
					actualname = "Type: Music";
					ImGui::Text(actualname.c_str());
					actualname = "Delete Event" + suffixLabel + events[i]->GetEventName();
					if (ImGui::Button(actualname.c_str()))
					{
						events[i]->toDelete = true;
					}
				}
			}
			ImGui::NextColumn();
			ImGui::SetColumnWidth(-1, 125);
			actualname = "Play" + suffixLabel + events[i]->GetEventName();
			if (ImGui::Button(actualname.c_str()))
			{
				App->audioManager->SendStopEvent(this->ID, events[i]->GetEventName());
				App->audioManager->SendAudioObjEvent(this->ID, events[i]->GetEventName());
			}
			ImGui::SameLine();
			actualname = "Stop" + suffixLabel + events[i]->GetEventName();
			if (ImGui::Button(actualname.c_str()))
			{
				App->audioManager->SendStopEvent(this->ID, events[i]->GetEventName());
			}
			ImGui::Columns();



		}
		if (disableButtons)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		ImGui::Spacing();
		ImGui::Separator();

		actualname = "Audio Volume" + suffixLabel;
		if (ImGui::SliderFloat(actualname.c_str(), &volume, 0.0f, 100.0f))
		{
			App->audioManager->ChangeRTPCValue(this->ID, "SourceVolume", volume);
		}

		actualname = "Seconds to change music" + suffixLabel;
		float auxMusicChangeTime = musicChangeTime;
		if (ImGui::InputFloat(actualname.c_str(), &auxMusicChangeTime, 1.0f, 2.0f, 3))
		{
			SetSecondsToChangeMusic(auxMusicChangeTime);
		}
		actualname = "Sound Pitch" + suffixLabel;
		float auxUserPitch = userPitch;
		if (ImGui::SliderFloat(actualname.c_str(), &auxUserPitch, 0.25f, 4.0f))
		{
			SetUserPitch(auxUserPitch);
		}

		ImGui::Separator();
		ImGui::Unindent();

		actualname = "Delete AudioSource Component" + suffixLabel;
		if (ImGui::BeginPopup(actualname.c_str(), ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("you are about to delete\n this component");
			actualname = "Go ahead" + suffixLabel;
			if (ImGui::Button(actualname.c_str()))
			{
				toDelete = true;
			}

			ImGui::SameLine();
			actualname = "Cancel" + suffixLabel;
			if (ImGui::Button(actualname.c_str()))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		float maxWidth = ImGui::GetWindowContentRegionMax().x;
		ImGui::SetCursorPosX(maxWidth - 50);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.25f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		actualname = "Delete" + suffixLabel + "component";
		if (ImGui::Button(actualname.c_str()))
		{
			actualname = "Delete AudioSource Component" + suffixLabel;
			ImGui::OpenPopup(actualname.c_str());


		}

		if (!activeAux)ImGui::PopStyleColor();
		ImGui::PopStyleColor(2);
	}
	if (!activeAux)ImGui::PopStyleColor();

}

bool AudioSource::Update(float dt)
{

	for (int i = events.size() - 1; i >= 0; i--)
	{
		if (events[i]->toDelete)
			DeleteAudioEvent(events[i]->GetEventName());
	}
	return false;
}

bool AudioSource::GameUpdate(float gameDT)
{
	if (owner != nullptr)
	{
		//Check reverb Zones to see if they affect this emitter
		CheckReverbZones();

		//Updates the RTPC variable that makes music blend possible
		musicTimeCounter += gameDT;
		if (musicTimeCounter >= musicChangeTime * 2)
		{
			musicTimeCounter = 0.0f;
		}

		float rtpcVal = 0.0f;
		if (musicChangeTime > 0.0f)
			rtpcVal = musicTimeCounter * (100.0f / (musicChangeTime * 2.0f));

		App->audioManager->ChangeRTPCValue(this->ID, "MusicBlendParameter", rtpcVal);

		//Updates the audio obj transform 
		ModuleTransform* transformComp = owner->GetComponent<ModuleTransform>();
		float4x4 transform = transformComp->GetGlobalTransform();

		App->audioManager->SetAudioObjTransform(this->ID, transform);

		//Changes audio pitch
		float overallPitch = userPitch * App->GetTimeScale();
		float res = RTPCValCalculationFromPitch(overallPitch);

		App->audioManager->ChangeRTPCValue(this->ID, "SoundPitch", res);

	}
	return true;
}

bool AudioSource::GameInit()
{
	//SAME AS GAME UPDATE FOR THE MOMENT
	if (owner != nullptr)
	{
		musicTimeCounter = 0.0f;

		ModuleTransform* transformComp = owner->GetComponent<ModuleTransform>();
		float4x4 transform = transformComp->GetGlobalTransform();

		App->audioManager->SetAudioObjTransform(this->ID, transform);



		for (int i = 0; i < events.size(); i++)
		{
			if (events[i]->GetPlayed() == false)
			{
				App->audioManager->SendAudioObjEvent(this->ID, events[i]->GetEventName());
				events[i]->StartPlaying();
			}
		}


	}

	return true;
}

float AudioSource::GetVolume() const
{
	return volume;
}

void AudioSource::SetVolume(float volume)
{
	this->volume = volume;
	App->audioManager->ChangeRTPCValue(ID, "SourceVolume", this->volume);
}

void AudioSource::CreateAudioEvent(std::string name, AudioEventType type, float changeTracksTime)
{
	if (!DoesAudioEventExist(name))
	{
		AudioEvent* newEv = new AudioEvent(name, type, changeTracksTime);
		events.push_back(newEv);
	}
	else
	{
		LOG("[error] Could not create an event with name: %s. Reason:", name.c_str());
		LOG("[error] There is an event with the same name already created in this component");
	}
}

void AudioSource::DeleteAudioEvent(std::string name)
{
	for (int i = 0; i < events.size(); i++)
	{
		if (events[i]->GetEventName() == name)
		{
			AudioEvent* auxEv = events[i];
			events.erase(events.begin() + i);
			delete auxEv;
			auxEv = nullptr;
			break;
		}
	}
}

bool AudioSource::DoesAudioEventExist(std::string name)
{
	for (int i = 0; i < events.size(); i++)
	{
		if (events[i]->GetEventName() == name)
		{
			return true;
		}
	}
	return false;
}

void AudioSource::DeleteAllAudioEvents()
{
	while (!events.empty())
	{
		AudioEvent* auxEv = events.back();
		events.pop_back();
		delete auxEv;
		auxEv = nullptr;
	}
}

void AudioSource::ResetAllAudioEvents()
{
	for (int i = 0; i < events.size(); i++)
	{
		events[i]->ResetPlayed();
	}
}

const std::vector<AudioEvent*> AudioSource::GetEvents() const
{
	return events;
}

void AudioSource::SetSecondsToChangeMusic(float newSeconds)
{
	musicChangeTime = max(0.0f, newSeconds);
	musicTimeCounter = 0.0f;//resets the counter every time it is changed
}

float AudioSource::GetSecondsToChangeMusic() const
{
	return musicChangeTime;
}

void AudioSource::SetUserPitch(float newPitch)
{
	userPitch = max(0.25f, newPitch);
	userPitch = min(userPitch, 4.0f);
}

float AudioSource::GetUserPitch() const
{
	return userPitch;
}

float AudioSource::RTPCValCalculationFromPitch(float overallPitch) const
{
	//overallPitch->0.25	== "SoundPitch"->0
	//overallPitch->1		== "SoundPitch"->100
	//overallPitch->4		== "SoundPitch"->200

	//1.01395949 this is the num with enough precision to make it nearly exact
	return Log(1.01396, overallPitch) + 100;
}

bool AudioSource::CheckReverbZones()
{
	bool ret = false;

	if (owner != nullptr)
	{
		float3 myPosition = owner->GetComponent<ModuleTransform>()->GetGlobalPosition();

		for (int i = 0; i < App->audioManager->revAreas.size(); i++)
		{

			float value = 0.0;
			if (App->audioManager->revAreas[i]->IsActive())
			{
				//if (App->audioManager->revAreas[i]->CheckCollision(t->GetTranslation()))
				if (App->audioManager->revAreas[i]->DoesReverbZoneContainPoint(myPosition))
				{
					value = App->audioManager->revAreas[i]->revValue;
					ret = true;
				}
			}
			ApplyReverb(value, App->audioManager->revAreas[i]->targetBus.c_str());
		}

		return ret;
	}
	else return ret;
}

void AudioSource::ApplyReverb(float revValue, const char* targetBus)
{
	if (App->audioManager->activeListener != nullptr)
	{
		App->audioManager->AuxSendValues(revValue, targetBus, App->audioManager->activeListener->GetResourceID(), this->ID);
	}

}
