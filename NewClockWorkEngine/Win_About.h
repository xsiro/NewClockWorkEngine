#include "Window.h"



class Win_About : public Window
{
public:
	Win_About(bool _active);
	virtual ~Win_About();

	void Draw() override;

	void CleanUp() override;


private:

};