#include "Window.h"
#include <vector>


class Win_Hierarchy : public Window
{
public:
	Win_Hierarchy(bool _active);
	virtual ~Win_Hierarchy();

	void Draw() override;

	void CleanUp() override;

	void GameObjectsHierarchy();


private:

};
