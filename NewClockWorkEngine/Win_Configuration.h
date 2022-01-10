#include "Window.h"
#include "imgui/include/ImGui.h"
#include <vector>
#include "Globals.h"


#define LOG_LENGTH 50

class Win_Configuration : public Window
{
public:

	Win_Configuration(bool _active);
	virtual ~Win_Configuration();

	void Init() override;
	void Draw() override;
	void CleanUp() override;

	void AddLogFPS(float fps, float ms);
	uint GetFR() const;
	void SetFRLimit(uint max_framerate);

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