#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleAudio.h"
#include "ModuleWindow.h"
#include "OpenGL.h"

#include <stdio.h>
#include "ImGui/include/imgui.h"
#include "ImGui/include/imgui_internal.h"
#include "ImGui/include/imgui_impl_opengl3.h"
#include "ImGui/include/imgui_impl_sdl.h"



ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Destructor
ModuleGui::~ModuleGui()
{}

// Called before render is available
bool ModuleGui::Awake()
{
	LOG("Loading GUI atlas");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool ModuleGui::Start()
{
	bool ret = true;

	// ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");


	return ret;
}

// Update all guis
update_status ModuleGui::PreUpdate(float dt)
{

	return  UPDATE_CONTINUE;
}

// Called every frame
update_status ModuleGui::Update(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	
	return UPDATE_CONTINUE;
}
update_status ModuleGui::PostUpdate(float dt)
{
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(App->window->window);
	return  UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}