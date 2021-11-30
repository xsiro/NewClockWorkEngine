#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleImporter.h"
#include "ModuleMesh.h"
#include "Primitive.h"
#include "GameObject.h"
#include "ModuleComponent.h"
#include "ModuleSceneIntro.h"

#include "Window.h"
#include "Win_Inspector.h"
#include "Win_Configuration.h"
#include "Win_Hierarchy.h"
#include "Win_Console.h"
#include "Win_About.h"

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
	
	cube = false;
	pyramid = false;
	cylinder = false;
	sphere = false;

	console = new Win_Console(true);
	about = new Win_About(false);
	hierarchy = new Win_Hierarchy(true);
	inspector = new Win_Inspector(true);
	config = new Win_Configuration((int)App->GetFRLimit(), true);

	AddWindow(console);
	AddWindow(inspector);
	AddWindow(about);
	AddWindow(hierarchy);
	AddWindow(config);
}

// Destructor
ModuleGui::~ModuleGui()
{
	

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
			if (ImGui::MenuItem("Exit", "(Alt+F4)"))
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
				config->SetActive();
			}
			if (ImGui::MenuItem("Console", " ", console->active))
			{
				console->SetActive();
			}
			if (ImGui::MenuItem("Hierarchy"))
			{
				hierarchy->SetActive();
			}
			if (ImGui::MenuItem("Inspector"))
			{
				inspector->SetActive();
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
				about->SetActive();
				
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create GameObject"))
		{
			if (ImGui::MenuItem("Cube")) {
				pyramid = false;
				cylinder = false;
				sphere = false;
				cube = !cube;
				if (cube)
				{
					LOG("Cube primitive created");
				}
			}
			if (ImGui::MenuItem("Pyramid")) {
				cube = false;
				cylinder = false;
				sphere = false;
				pyramid = !pyramid;
				if (pyramid)
				{
					LOG("Pyramid primitive created")
				}
			}
			if (ImGui::MenuItem("Cylinder")) {
				pyramid = false;
				cube = false;
				sphere = false;
				cylinder = !cylinder;
				if (cylinder)
				{
					LOG("Cylinder primitive created")
				}
			}
			if (ImGui::MenuItem("Sphere")) {
				pyramid = false;
				cylinder = false;
				cube = false;
				sphere = !sphere;
				if (sphere)
				{
					LOG("Sphere primitive created")
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	//Here we create the different windows

	std::vector<Window*>::iterator item = winArray.begin();

	for (item; item != winArray.end(); ++item)
	{
		(*item)->Draw();
	}



	return UPDATE_CONTINUE;
}
update_status ModuleGui::PostUpdate(float dt)
{

	ImGui::Render();


	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	return  UPDATE_CONTINUE;
}

bool ModuleGui::CleanUp()
{
	std::vector<Window*>::iterator item = winArray.begin();
	for (item; item != winArray.end(); ++item)
		(*item)->CleanUp();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
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



void ModuleGui::AddWindow(Window* window)
{
	winArray.push_back(window);
}

void ModuleGui::Log(char* text)
{
	if (console != nullptr)
		console->ConsoleLog(text);
}

void ModuleGui::LogFPS(float fps, float ms)
{
	if (config != nullptr)
		config->AddLogFPS(fps, ms);
}

bool ModuleGui::GetVL()
{
	return inspector->vertexlines;
}

bool ModuleGui::GetFL()
{
	return inspector->facelines;
}

bool ModuleGui::GetCheck()
{
	return inspector->check;
}

bool ModuleGui::GetWireframe()
{
	return inspector->wireframe;
}

