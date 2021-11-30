#include "Win_Inspector.h"
#include "OpenGL.h"
#include <stdio.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "glew.h"
#include "SDL_opengl.h"
#include "Globals.h"
#include "Application.h"


Win_Inspector::Win_Inspector(bool _active) : Window(_active)
{
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


Win_Inspector::~Win_Inspector()
{}

void Win_Inspector::Draw()
{
	if (!active)
		return;

	ImGui::Begin("Inspector", &active);
	
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

			ImGui::Separator();
			ImGui::Text("Texture:");
			ImGui::SameLine();
			ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "%s", App->importer->GetMaterialFileName());
			ImGui::Separator();
			ImGui::Text("General");
			ImGui::Text("");
			if (ImGui::Checkbox("Cube Map", &cubemap))
			{
				App->renderer3D->SetCubemap(cubemap);
			}

			if (ImGui::Checkbox("Checker Mode", &check));
		}
		ImGui::End();
	

}

void Win_Inspector::CleanUp()
{
}