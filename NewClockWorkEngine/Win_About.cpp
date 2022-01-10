#include "Win_About.h"
#include "OpenGL.h"
#include <stdio.h>
#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_opengl3.h"
#include "imgui/include/imgui_impl_sdl.h"
#include "glew/include/glew.h"
#include "sdl/include/SDL.h"
#include "Globals.h"
#include "Application.h"


Win_About::Win_About(bool _active) : Window(_active)
{}


Win_About::~Win_About()
{}

void Win_About::Init()
{

}

void Win_About::Draw()
{
	if (!active)
		return;

	if (ImGui::Begin("About", &active))
	{
		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		ImGui::Text("ClockWorkEngine v0.1");
		ImGui::Text("ClockWorkEngine is developed by Daniel Ruiz & Alex Lopez");
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
		ImGui::Text("Copyright (c) 2021 [Daniel Ruiz & Alex Lopez]");
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

		if (ImGui::IsWindowHovered())
			App->gui->mouseHovered = true;
		ImGui::End();
	}
	
}

void Win_About::CleanUp()
{
}