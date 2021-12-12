#pragma once

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow( bool start_enabled = true);

	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	float GetBrightness();
	void SetBright(float bright);
	uint GetWidth() const;
	uint GetHeight() const;
	void SetWidth(int width);
	void SetHeight(int height);
	void SetFullScreen(bool state);
	void SetBorderless(bool state);
	void SetFullDesktop(bool state);
	void SetTitle(const char* title);

public:
	SDL_Window* window;
	bool fullscreen;
	bool fullscreen_desktop;
	bool resizable;
	bool borderless;
	float brightness;
	int width;
	int height;

	SDL_Surface* screen_surface;
	
};
