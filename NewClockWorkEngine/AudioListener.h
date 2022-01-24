#ifndef __C_AUDIO_LISTENER__
#define __C_AUDIO_LISTENER__

#include "Component.h"


class AudioListener :public Component
{
public:
	AudioListener(GameObject* owner, unsigned int ID);
	~AudioListener();
	void SetAsListener(bool newState);
	bool GetIsListener()const;
	void OnEditor();
	bool GameUpdate(float gameDT)override;
	bool GameInit()override;

private:
	bool isListener;

};

#endif // !__C_AUDIO_LISTENER__

