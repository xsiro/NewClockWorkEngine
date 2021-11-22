#ifndef __WINDOW__
#define __WINDOW__

class Window
{
public:

	Window(bool _active);
	~Window();

	void SetActive();

	virtual void Draw() = 0;

	inline bool IsActive() const { return active; }

	virtual void CleanUp() = 0;

public:
	bool active = true;
};

#endif //__WINDOW__