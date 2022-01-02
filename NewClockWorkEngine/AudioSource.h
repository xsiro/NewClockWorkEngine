#ifndef __AUDIO_SOURCE__
#define __AUDIO_SOURCE__

#include "ModuleComponent.h"


class AudioSource :public ModuleComponent
{
public:
	AudioSource(GameObject* owner, unsigned int ID);
	~AudioSource();

	void OnEditor();

private:

};

#endif // !__AUDIO_SOURCE__


