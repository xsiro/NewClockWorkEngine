#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleImporter.h"
#include "Primitive.h"
#include "GameObject.h"
#include "ModuleComponent.h"
#include "ModuleSceneIntro.h"

#include "OpenGL.h"
#include <stdio.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "glew.h"
#include "SDL_opengl.h"



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
	

	depthtest = false;
	cullface = false;
	lighting = false;
	material = false;
	cubemap = true;
	polygonssmooth = false;

	wireframe = false;
	vertexlines = false;
	facelines = false;
	check = false;


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
bool ModuleGui::Init()
{
	bool ret = true;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	gl_context = SDL_GL_CreateContext(App->window->window);
	//SDL_GL_MakeCurrent(App->window->window, gl_context);
	
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);

	return ret;
}

// Update all guis
update_status ModuleGui::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	return  UPDATE_CONTINUE;
}

// Called every frame
update_status ModuleGui::Update(float dt)
{
	Dock(dockingwindow);
	
	
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
			if (ImGui::MenuItem("Hierarchy"))
			{
				hierarchy = !hierarchy;
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

	if (inspector)
	{
		ImGui::Begin("Inspector", &inspector);
		ImGui::Text("Inspector");
		if (ImGui::CollapsingHeader("Local Transformation"))
		{
			ImGui::Separator();
			vec3 position = { 0,0,0 };
			vec3 rotation = { 0,0,0 };
			vec3 scale = { 0,0,0 };
			if (ImGui::DragFloat3("Position", &position, 0.1f))
			{

			}

			if (ImGui::DragFloat3("Rotation", &rotation, 0.1f))
			{

			}

			if (ImGui::DragFloat3("Scale", &scale, 0.1f))
			{

			}
		}
		if (ImGui::CollapsingHeader("Mesh"))
		{
			ImGui::Separator();
			ImGui::Text("File:");
			ImGui::SameLine();
			ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "%s", App->importer->GetMeshFileName());
			ImGui::Separator();
			ImGui::Text("General");
			ImGui::Text("");
			if (ImGui::Checkbox("Wireframe", &wireframe));

			if (ImGui::Checkbox("See Vertex Lines (Blue)", &vertexlines));

			if (ImGui::Checkbox("See Face Lines (Green)", &facelines));

			if (ImGui::Checkbox("Depth Test", &depthtest)) {
				App->renderer3D->SetDepthtest(depthtest);
			}
			if (ImGui::Checkbox("Cull Face", &cullface)) {
				App->renderer3D->SetCullface(cullface);
			}
			if (ImGui::Checkbox("Lightning", &lighting)) {
				App->renderer3D->SetLighting(lighting);
			}
			ImGui::Text("");
			ImGui::Text("Polygons smoothing");
			ImGui::Text("");
			if (ImGui::Checkbox("Polygons smooth", &polygonssmooth))
			{
				App->renderer3D->SetPolygonssmooth(polygonssmooth);
			}
		}
		if (ImGui::CollapsingHeader("Material"))
		{
			ImGui::Text("Textures");
			ImGui::Text("");
			if (ImGui::Checkbox("Cube Map", &cubemap))
			{
				App->renderer3D->SetCubemap(cubemap);
			}

			if (ImGui::Checkbox("Checker Mode", &check));
		}
		ImGui::End();
	}

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

			ImGui::SliderInt("Width", &width, 720, 1920);
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
		if (ImGui::CollapsingHeader("Software"))
		{
			GLint major, minor;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);

			ImVec4 color(1.0f, 0.0f, 1.0f, 1.0f);

			ImGui::BulletText("SDL ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
			ImGui::BulletText("ImGui ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%s", ImGui::GetVersion());
			ImGui::BulletText("Glew ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
			ImGui::BulletText("OpenGL ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
			ImGui::BulletText("DeviL ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d", major, minor);
			ImGui::BulletText("MathGeoLib");
			ImGui::SameLine();
			ImGui::TextColored(color, "1.5");
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

	if (about_window)
	{
		if (ImGui::Begin("About", &about_window))
		{
			GLint major, minor;
				glGetIntegerv(GL_MAJOR_VERSION, &major);
				glGetIntegerv(GL_MINOR_VERSION, &minor);

				ImGui::Text("ClockWorkEngine v0.1");
				ImGui::Text("ClockWorkEngine is developed by Daniel Ruiz & Pol Cortes");
				ImGui::Text("This engine has been coded in C++");
				ImGui::Text("Libraries used: ");
				ImVec4 color(1.0f, 0.0f, 0.0f, 1.0f);

				ImGui::BulletText("SDL ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
			ImGui::BulletText("ImGui ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%s", ImGui::GetVersion());
			ImGui::BulletText("Glew ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
			ImGui::BulletText("OpenGL ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
			ImGui::BulletText("DeviL ");
			ImGui::SameLine();
			ImGui::TextColored(color, "%d.%d", major, minor);
			ImGui::BulletText("MathGeoLib");
			ImGui::SameLine();
			ImGui::TextColored(color, "1.5");

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

	if (show_console)
	{
		
		if (ImGui::Begin("Console", &show_console,ImGuiWindowFlags_None))
		{
			for (uint i = 0; i < logs.size(); ++i)
			{
				ImGui::TextUnformatted(logs[i]);
			}
			ImGui::End();
			
		}
	}
	if (hierarchy)
	{
		ImGui::Begin("Hierarchy", &hierarchy);

		if (ImGui::Button("Delete"))
		{
			App->scene_intro->CleanUp();
		}
		GameObjectsHierarchy();
		ImGui::End();
	}


	return UPDATE_CONTINUE;
}
update_status ModuleGui::PostUpdate(float dt)
{

	ImGui::Render();


	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	return  UPDATE_CONTINUE;
}

//void ModuleGui::Draw() {
//
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//}

bool ModuleGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	ClearLog();


	return true;
}

void ModuleGui::ConsoleLog(char* logStr)
{
	if (logs.size() + 1 > THRESHOLD_LOGS)
	{
		ClearLog();
	}

	char* tmp = _strdup(logStr);

	logs.push_back(tmp);
}

void ModuleGui::ClearLog()
{
	for (int i = 0; i < logs.size(); ++i)
	{
		free(logs[i]);
	}
	logs.clear();
}

update_status ModuleGui::Dock(bool* p_open)
{
	update_status ret = UPDATE_CONTINUE;

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();

	return ret;
}

void ModuleGui::GameObjectsHierarchy()
{

	if (ImGui::TreeNode("GameObject")) {
		for (size_t i = 0; i < App->scene_intro->game_objects.size(); i = i + 2)
		{
			if (App->scene_intro->game_objects[i]->parent != nullptr) continue;

			if (ImGui::TreeNodeEx(App->scene_intro->game_objects[i]->name.c_str(), ImGuiTreeNodeFlags_Leaf)) {
				if (ImGui::IsItemClicked()) {
					App->scene_intro->selected = App->scene_intro->game_objects[i];
				}
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}


}
