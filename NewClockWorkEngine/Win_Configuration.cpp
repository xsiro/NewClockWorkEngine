
#include "Win_Configuration.h"
#include "OpenGl.h"
#include "ImGui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Color.h"
#include "Globals.h"
#include <vector>

using namespace std;

// ---------------------------------------------------------
Win_Configuration::Win_Configuration(int _max_fps, bool _active) : Window(_active),
fps_log(LOG_LENGTH), ms_log(LOG_LENGTH)
{
	width = 1280;
	height = 1024;
	brightness = 1.0f;
	fullscreen = false;
	resizable = false;
	borderless = false;
	fulldesktop = false;
	fps = 0;
}

Win_Configuration::~Win_Configuration()
{}

void Win_Configuration::CleanUp()
{
}


void Win_Configuration::AddLogFPS(float fps, float ms)
{
	static uint count = 0;

	if (count == LOG_LENGTH)
	{
		for (uint i = 0; i < LOG_LENGTH - 1; ++i)
		{
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}
	}
	else
		++count;

	fps_log[count - 1] = fps;
	ms_log[count - 1] = ms;
}

void Win_Configuration::Draw()
{
	if (!active)
		return;
		
	if (ImGui::Begin("Configuration", &active));
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

		ImGui::Checkbox("Borderless", &borderless);
		App->window->SetBorderless(borderless);
		ImGui::SameLine();

		ImGui::Checkbox("Fulldesktop", &fulldesktop);
		App->window->SetFullDesktop(fulldesktop);

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
		ImVec4 color(1.0f, 0.0f, 1.0f, 1.0f);

		ImGui::Text("Mouse X:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%d", App->input->GetMouseX());
		ImGui::Text("Mouse Y:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%d", App->input->GetMouseY());

		ImGui::Spacing();

		ImGui::Text("Current Window Mouse X: %.2f", App->gui->mouseScenePosition.x);
		ImGui::Text("Current Window Mouse Y: %.2f", App->gui->mouseScenePosition.y);

		ImGui::Spacing();

	}

	ImGui::End();
}