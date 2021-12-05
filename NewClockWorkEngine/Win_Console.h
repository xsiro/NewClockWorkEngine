
#include "Window.h"
#include <vector>

#define THRESHOLD_LOGS 1000


class Win_Console : public Window
{
public:
	Win_Console(bool _active);
	virtual ~Win_Console();

	void Init() override;
	void Draw() override;

	void CleanUp() override;
	void ConsoleLog(char* logStr);
	void ClearLog();

private:

	std::vector<char*> logs;
	std::vector<char*> console_log;
};
