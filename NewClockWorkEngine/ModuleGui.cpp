#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleAudio.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include <stdio.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"



ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	brightness = 1.0f;
	width = 1280;
	height = 1024;
	fps = 0;
	fullscreen = false;
	resizable = false;
	borderless = true;
	fulldesktop = false;
	fps_log = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	ms_log = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

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
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	
	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				return UPDATE_STOP;
				
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if(ImGui::MenuItem("Demo menu"))
			{ 
				show_demo_window = !show_demo_window;
			}
			if (ImGui::MenuItem("Configuration menu"))
			{
				show_config = !show_config;
			}
			
			ImGui::EndMenu();

		}


		ImGui::EndMainMenuBar();
	}

	//Here we create the different windows

	if (show_config)
	{
		if (ImGui::Begin("Configuration", &show_config));
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Set defaults"))
				{
					UPDATE_CONTINUE;
				}
				if (ImGui::MenuItem("Load"))
				{
					UPDATE_CONTINUE;
				}
				if (ImGui::MenuItem("Save"))
				{
					UPDATE_CONTINUE;
				}
				ImGui::EndMenu();
			}
			
		}
		if (ImGui::CollapsingHeader("Application"))
		{
			char Appname[32] = "ClockWork Engine";
			ImGui::InputText("App Name", Appname, IM_ARRAYSIZE(Appname));

			char Organization[32] = "CITM";
			ImGui::InputText("Organization", Organization, IM_ARRAYSIZE(Organization));

			ImVec4 color(1.0f, 0.0f, 1.0f, 1.0f);
			int mfps = 1000 / App->max_ms;

			if (ImGui::SliderInt("Max FPS", &mfps, 10, 120)) App->max_ms = 1000 / mfps;
		
			ImGui::Text("Limit Framerate: ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%i", fps);
			fps_log.erase(fps_log.begin());
			fps_log.push_back(App->fps);
			ms_log.erase(ms_log.begin());
			ms_log.push_back(App->dt * 1000);

			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
		}
		if (ImGui::CollapsingHeader("Window"))
		{

			ImGui::Text("Icon:  *default");

			ImGui::SliderFloat("Brightness", &brightness, 0.f, 1.0f);
			App->window->SetBright(brightness);

			if (ImGui::Checkbox("Fullscreen", &fullscreen));
			App->window->SetFullScreen(fullscreen);
			ImGui::SameLine();

		/*	ImGui::Checkbox("Resizable", &resizable);
			App->window->SetResizable(resizable);*/

			ImGui::Checkbox("Borderless", &borderless);
			App->window->SetBorderless(borderless);
			ImGui::SameLine();

			ImGui::Checkbox("Fulldesktop", &fulldesktop);
			App->window->SetFullDesktop(fulldesktop);

			if (ImGui::IsAnyItemHovered())
				ImGui::SetTooltip("Restart to Apply");
		}

		ImGui::End();
	}

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

	return  update_status();
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