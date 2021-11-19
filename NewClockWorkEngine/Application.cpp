#include "Application.h"


Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene_intro = new ModuleSceneIntro(this);	
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGui(this);
	importer = new ModuleImporter(this);
	filesystem = new FileSystem(this);
	
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(gui);
	AddModule(importer);
	AddModule(filesystem);
	
	// Scenes
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);
	max_ms = 1000 / 60;
	fps = 0;
}

Application::~Application()
{
	std::vector<Module*>::reverse_iterator item = list_modules.rbegin();
	while (item != list_modules.rend())
	{
		delete* item;
		item++;
	}
}

bool Application::Init()
{
	bool ret = true;
	LOG("Application Start --------------");
	// Call Init() in all modules
	for (int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	
	LOG("Application Start --------------");
	for (int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Start();
	}

	LOG("Engine Info-----------------------");
	//LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ms_timer.Start();

	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
	fps = 1.0f / dt;
}

// ---------------------------------------------
void Application::FinishUpdate()
{

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::vector<Module*>::reverse_iterator item = list_modules.rbegin();
	
	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->Update(dt);
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	
	FinishUpdate();
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