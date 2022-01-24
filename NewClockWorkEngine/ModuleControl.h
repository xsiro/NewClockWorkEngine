#ifndef __MODULE_CONTROL__
#define __MODULE_CONTROL__

#include "Component.h"

class ModuleControl :public Component
{
public:
	ModuleControl(GameObject* owner, unsigned int ID);
	~ModuleControl();
	void OnEditor();
	bool GameUpdate(float gameDT)override;
	bool GameInit()override;
	bool ManageMovement(float gameDT);
	bool Update(float dt);
	float GetSpeed()const;
	void SetSpeed(float newSpeed);
private:
	float speed;
};

#endif // !__MODULE_CONTROL__
