#include "AudioEvent.h"
#include "Globals.h"

AudioEvent::AudioEvent(std::string eventName, AudioEventType type, float changeTrackTime) :
	eventName(eventName), type(type), hasPlayed(false), toDelete(false)
{
}

AudioEvent::~AudioEvent()
{
	toDelete = true;
	hasPlayed = false;
	eventName.clear();
	type = AudioEventType::UNKNOWN;
}

std::string AudioEvent::GetEventName() const
{
	return eventName;
}

void AudioEvent::ResetPlayed()
{
	hasPlayed = false;
}

bool AudioEvent::GetPlayed() const
{
	return hasPlayed;
}

void AudioEvent::StartPlaying()
{
	hasPlayed = true;
}

AudioEventType AudioEvent::GetType() const
{
	return type;
}
