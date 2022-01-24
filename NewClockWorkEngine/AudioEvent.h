#ifndef __AUDIO_EVENT_H__
#define __AUDIO_EVENT_H__
#include <string>
enum class AudioEventType
{
	SFX,
	MUSIC,
	UNKNOWN
};

class AudioEvent
{
public:
	AudioEvent(std::string eventName, AudioEventType type, float changeTrackTime = 0.0f);
	~AudioEvent();
	std::string GetEventName()const;
	bool GetPlayed()const;
	void StartPlaying();
	void ResetPlayed();
	AudioEventType GetType()const;

private:
	bool hasPlayed; //this bool shows whether the audio has been played or not

	AudioEventType type;
	std::string eventName;
public:
	bool toDelete;
};


#endif // !__AUDIO_EVENT_H__

