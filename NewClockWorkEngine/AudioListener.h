#ifndef __AUDIO_LISTENER__
#define __AUDIO_LISTENER__

#include "ModuleComponent.h"


class AudioListener :public ModuleComponent
{
public:
	AudioListener(GameObject* owner, unsigned int ID);
	~AudioListener();

	void SetAsListener(bool newState);
	bool GetIsListener()const;

	void DrawInspector() override;

private:
	bool isListener;
};

#endif // !__AUDIO_LISTENER__

