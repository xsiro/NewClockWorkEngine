
#pragma once

enum WindowType {
	HIERARCHY_WINDOW,
	INSPECTOR_WINDOW,
	SCENE_WINDOW,
	//CONSOLE_WINDOW,
	ASSETS_WINDOW,
	CONFIGURATION_WINDOW,
	ABOUT_WINDOW,
	IMPORT_WINDOW,
	MAX_WINDOWS
};

class Window {
public:
	Window() : visible(false) {};
	virtual ~Window() {};
	virtual void Draw() = 0;

public:
	bool visible;
	WindowType type = MAX_WINDOWS;
};
