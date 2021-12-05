#include "Window.h"
#include "ImGui.h"
#include <vector>


#define LOG_LENGTH 50

class Win_Configuration : public Window
{
public:

	Win_Configuration(int _max_fps, bool _active);
	virtual ~Win_Configuration();

	void Init() override;
	void Draw() override;
	void CleanUp() override;

	void AddLogFPS(float fps, float ms);

	bool changeFPSlimit = false;
	bool changeTitleName = false;
	int max_fps = 140;

private:
	ImGuiTextBuffer input_buf;
	bool need_scroll = false;
	std::vector<float> fps_log;
	std::vector<float> ms_log;

public:
	int fps;
	int height;
	int width;

	float brightness;

	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fulldesktop;
};