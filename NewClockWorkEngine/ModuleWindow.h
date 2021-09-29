#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	float GetBrightness();
	void SetBright(float bright);
	void GetSize(int& width, int& height);
	void SetSize(int width, int height);
	void SetFullScreen(bool state);
	void SetBorderless(bool state);
	void SetResizable(bool state);
	void SetFullDesktop(bool state);
	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;
	bool fullscreen;
	bool fullscreen_desktop;
	bool resizable;
	bool borderless;
	float brightness;
	int width;
	int height;
	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__