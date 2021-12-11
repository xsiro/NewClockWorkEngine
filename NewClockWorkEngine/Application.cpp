#include "Application.h"



Application::Application() : debug(false), dt(0.16f)
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGui(this);
	filesystem = new FileSystem(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(gui);
	AddModule(filesystem);

	// Scenes
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);

	closewindow = false;
	contFPS = 0;
	frames = 0;
	miliseconds = 1000 / 60;
	last_fps = -1;
	last_ms = -1;
}

Application::~Application()
{
	std::vector<Module*>::iterator item = list_modules.end();
	for (; item != list_modules.begin(); --item) {
		delete (*item);
	}
}

bool Application::Init()
{
	bool ret = true;
	App = this;
	LOG("Application Start --------------");
	// Call Init() in all modules
	std::vector<Module*>::iterator item = list_modules.begin();

	for (; item != list_modules.end() && ret == true; ++item) {
		ret = (*item)->Init();
	}

	// After all Init calls we call Start() in all modules

	LOG("Application Start --------------");
	item = list_modules.begin();

	for (; item != list_modules.end() && ret == true; ++item) {
		ret = (*item)->Start();
	}

	LOG("Engine Info-----------------------");
	//LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = (float)frame_time.Read() / 1000.0f;
	frame_time.Start();

	if (!GameMode || GamePaused)
		Game_dt = 0.0f;
	else
		Game_dt = dt;
	Game_dt *= GameSpeed;
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	++frames;
	++contFPS;

	if (fps_timer.Read() >= 1000)
	{
		last_fps = contFPS;
		contFPS = 0;
		fps_timer.Start();
	}

	last_ms = ms_timer.Read();

	if (miliseconds > 0 && (last_ms < miliseconds))
	{
		SDL_Delay(miliseconds - last_ms);
	}
	gui->LogFPS((float)last_fps, (float)last_ms);
}
// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	std::vector<Module*>::iterator item = list_modules.begin();

	//BROFILER_CATEGORY("Engine PreUpdate", Profiler::Color::Yellow)
	for (; item != list_modules.end() && ret == UPDATE_CONTINUE; ++item)
	{
		ret = (*item)->PreUpdate(dt);
	}

	item = list_modules.begin();

	//BROFILER_CATEGORY("Engine Update", Profiler::Color::Green)
	for (; item != list_modules.end() && ret == UPDATE_CONTINUE; ++item)
	{
		ret = (*item)->Update(dt);
	}

	item = list_modules.begin();

	//BROFILER_CATEGORY("Engine PostUpdate", Profiler::Color::Purple)
	for (; item != list_modules.end() && ret == UPDATE_CONTINUE; ++item)
	{
		ret = (*item)->PostUpdate(dt);
	}

	//BROFILER_CATEGORY("Sleep", Profiler::Color::Blue)
	FinishUpdate();

	if (closewindow)
		ret = update_status::UPDATE_STOP;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (size_t i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->CleanUp();
	}


	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::PlayGame()
{
	if (!GameMode)
	{
		GameMode = true;
	}
}

void Application::PauseGame()
{
	if (GameMode)
	{
		GamePaused = true;
	}
}

void Application::ResumeGame()
{
	if (GameMode && GamePaused)
	{
		GamePaused = false;
	}
}

void Application::StopPlay()
{
	if (GameMode)
	{
		GameMode = false;
		GamePaused = false;
	}
}
int Application::CPUCount()
{
	return SDL_GetCPUCount();
}

int Application::CPUCache()
{
	return SDL_GetCPUCacheLineSize();
}

int Application::SystemRAM()
{
	int TransformtoGB = SDL_GetSystemRAM() * 0.001;
	return TransformtoGB;
}
void Application::RequestBrowser(const char* url)const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

const char* Application::SystemCaps()
{
	Caps.clear();
	// IF the processor has certain register it will be added to the string
	if (SDL_Has3DNow())
	{
		Caps.append("3D Now, ");
	}

	if (SDL_HasAVX())
	{
		Caps.append("AVX, ");
	}

	if (SDL_HasAVX2())
	{
		Caps.append("AVX2, ");
	}

	if (SDL_HasAltiVec())
	{
		Caps.append("AltiVec, ");
	}

	if (SDL_HasMMX())
	{
		Caps.append("MMX, ");
	}

	if (SDL_HasRDTSC())
	{
		Caps.append("RDTSC, ");
	}

	if (SDL_HasSSE())
	{
		Caps.append("SSE, ");
	}

	if (SDL_HasSSE2())
	{
		Caps.append("SSE2, ");
	}

	if (SDL_HasSSE3())
	{
		Caps.append("SSE3, ");
	}

	if (SDL_HasSSE41())
	{
		Caps.append("SSE41, ");
	}

	if (SDL_HasSSE41())
	{
		Caps.append("SSE42");
	}

	return Caps.data();
}

const char* Application::Brand() {
	return (const char*)glGetString(GL_VENDOR);
}

const char* Application::Model() {
	return (const char*)glGetString(GL_RENDERER);
}

int Application::Budget() {
	int budget;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &budget);
	return budget / 1024;
}

int Application::Usage() {
	int usage;
	glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &usage);
	return usage / 1024;
}

int Application::Available() {
	int available;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &available);
	return available / 1024;
}

int Application::Reserved() {
	int reserved;
	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &reserved);
	return reserved / 1024;
}

uint Application::GetFRLimit() const
{
	if (miliseconds > 0)
		return (uint)((1.0f / (float)miliseconds) * 1000.0f);
	else
		return 0;
}

void Application::SetFRLimit(uint max_framerate)
{
	if (max_framerate > 0)
		miliseconds = 1000 / max_framerate;
	else
		miliseconds = 0;
}

void Application::ExitApp()
{
	closewindow = true;
}

Application* App = nullptr;