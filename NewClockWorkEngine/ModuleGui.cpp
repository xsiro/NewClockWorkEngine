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
#include "Glew/include/glew.h"




ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	brightness = 1.0f;
	width = 1280;
	height = 1024;
	fullscreen = false;
	resizable = false;
	borderless = true;
	fulldesktop = false;
	about_window = false;
	fps = 0;
	fps_log = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	ms_log = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	
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
	App->renderer3D->OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);


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
			if (ImGui::MenuItem("Console"))
			{
				show_console = !show_console;
			}
			
			ImGui::EndMenu();

		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo"))
			{
				show_demo_window = !show_demo_window;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				App->RequestBrowser("https://github.com/xsiro/NewClockWorkEngine/wiki");
			}
			if (ImGui::MenuItem("Download latest"))
			{
				App->RequestBrowser("https://github.com/xsiro/NewClockWorkEngine");
			}
			if (ImGui::MenuItem("About"))
			{
				about_window = !about_window;
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

			ImGui::SliderFloat("Brightness", &brightness, 0.f, 1.0f);
			App->window->SetBright(brightness);

			ImGui::SliderInt("Width", &width, 640, 1920);
			App->window->SetWidth(width);

			ImGui::SliderInt("Height", &height, 480, 1080);
			App->window->SetHeight(height);

			if (ImGui::Checkbox("Fullscreen", &fullscreen));
			App->window->SetFullScreen(fullscreen);
			ImGui::SameLine();

			/*ImGui::Checkbox("Resizable", &resizable);
			App->window->SetResizable(resizable);*/

			ImGui::Checkbox("Borderless", &borderless);
			App->window->SetBorderless(borderless);
			ImGui::SameLine();

			ImGui::Checkbox("Fulldesktop", &fulldesktop);
			App->window->SetFullDesktop(fulldesktop);

			if (ImGui::IsAnyItemHovered())
				ImGui::SetTooltip("Restart to Apply");
		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImVec4 color(1.0f, 1.0f, 0.0f, 1.0f);
			ImGui::Text("SDL version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
			ImGui::Separator();

			int cpu;
			int ram;
			int cache;
			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%i", cpu = App->CPUCount());
			ImGui::SameLine();
			ImGui::TextColored(color, "(Cache: %iKb)", cache = App->CPUCache());
			ImGui::Text("System RAM: ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%i.0GB", ram = App->SystemRAM());
			ImGui::Text("Caps: ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%s", App->SystemCaps());
			ImGui::Separator();

			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%s", App->Brand());
			ImGui::Text("Model:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%s", App->Model());
			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%i Mb", App->Budget());
			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%i Mb", App->Usage());
			ImGui::Text("VRAM Available:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%i Mb", App->Available());
			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored(color, "%i Mb", App->Reserved());
		}
		if (ImGui::CollapsingHeader("Input"))
		{
			ImGui::Text("Mouse X: %d", App->input->GetMouseX());
			ImGui::Text("Mouse Y: %d", App->input->GetMouseY());

			ImGui::Spacing();

			ImGui::Text("Current Window Mouse X: %.2f", App->gui->mouseScenePosition.x);
			ImGui::Text("Current Window Mouse Y: %.2f", App->gui->mouseScenePosition.y);

			ImGui::Spacing();

			ImGui::Text("Normalized Mouse X: %.2f", App->gui->mouseScenePosition.x / App->gui->image_size.x);
			ImGui::Text("Normalized Mouse Y: %.2f", App->gui->mouseScenePosition.y / App->gui->image_size.y);

			ImGui::Spacing();

			float normalized_x = App->gui->mouseScenePosition.x / App->gui->image_size.x;
			float normalized_y = App->gui->mouseScenePosition.y / App->gui->image_size.y;

			normalized_x = (normalized_x - 0.5f) * 2.0f;
			normalized_y = -(normalized_y - 0.5f) * 2.0f;

			ImGui::Text("Near Plane Mouse X: %.2f", normalized_x);
			ImGui::Text("Near Plane Mouse Y: %.2f", normalized_y);
		}

		ImGui::End();
	}
	if (show_console)
	{
		
		if (ImGui::Begin("Console", &show_console))
		{
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) LOG("Console working");

			ImGui::End();
		}
		else
		{
			std::vector<char*>::iterator item = logs.begin();
			for (item; item != logs.end(); ++item)
			{
				ImGui::TextUnformatted((*item));
			}
			if (scroll)
			{
				//ImGui::SetScrollHere(1.0f);
				scroll = false;
			}
			ImGui::End();
		}
	}
	if (about_window)
	{
		if (ImGui::Begin("About", &about_window))
		{
			ImGui::Text("ClockWorkEngine v0.1");
			ImGui::Text("ClockWorkEngine is developed by Daniel Ruiz & Pol Cortes");
			ImGui::Text("This engine has been coded in C++");
			ImGui::Text("3rd party libraries used:");
			ImGui::BulletText("SDL 2.06");
			ImGui::BulletText("Glew 2.0.0");
			ImGui::BulletText("ImGui");
			ImGui::BulletText("MathGeoLib");

			ImGui::Text("");

			ImGui::Text("LICENSE:");
			ImGui::Text("");
			ImGui::Text("MIT License");
			ImGui::Text("");
			ImGui::Text("Copyright (c) 2021 [Daniel Ruiz & Pol Cortes]");
			ImGui::Text("");
			ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
			ImGui::Text("of this software and associated documentation files (the 'Software'), to deal");
			ImGui::Text("in the Software without restriction, including without limitation the rights");
			ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
			ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
			ImGui::Text("furnished to do so, subject to the following conditions:");
			ImGui::Text("");
			ImGui::Text("The above copyright notice and this permission notice shall be included in all");
			ImGui::Text("copies or substantial portions of the Software.");
			ImGui::Text("");
			ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
			ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
			ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE");
			ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
			ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
			ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN ");
			ImGui::Text("THE SOFTWARE.");
			ImGui::End();
		}
		
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
	//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(App->window->window);

	return  update_status();
}

bool ModuleGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	ClearLog();
	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}

void ModuleGui::ConsoleLog(char* log)
{
	logs.push_back(strdup(log));
	scroll = true;
}
void ModuleGui::ClearLog()
{
	for (int i = 0; i < logs.size(); ++i)
	{
		free(logs[i]);
	}
	logs.clear();
}

const char* ModuleGui::GetName() const
{
	return name;
}