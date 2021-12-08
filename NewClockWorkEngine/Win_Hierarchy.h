#include "Window.h"
#include <vector>

class Window;
class GameObject;
class Win_Hierarchy : public Window
{
public:
	Win_Hierarchy(bool _active);
	virtual ~Win_Hierarchy();
	void Init() override;
	void Draw() override;

	void CleanUp() override;

	void GameObjectsHierarchy(GameObject* object);


private:

};
