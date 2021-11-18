#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "ModuleImporter.h"
#include "ModuleMesh.h"
#include "glmath.h"
#include "glew.h"
#include "SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>


#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	context = nullptr;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context != NULL)
	{
		LOG("OpenGL context successfully created")
	}
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();
	if (error == GL_NO_ERROR)
	{
		LOG("Successfully initializated glew library");
	}
	if (error != GL_NO_ERROR)
	{
		LOG("Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error == GL_NO_ERROR)
		{
			LOG("Successfully initializated OpenGL library")
		}
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

	}

	// Projection matrix for
	OnResize(App->gui->width, App->gui->height);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	Color c = App->camera->background;
	glClearColor(c.r, c.g, c.b, c.a);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	/*RenderFBX();

	if (App->gui->vertexlines)
	{
		DrawVertexNormalLines();
	}
	if (App->gui->facelines)
	{
		DrawFaceNormalLines();
	}*/
	App->gui->PostUpdate(dt);
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::SetDepthtest(bool state) {
	if (state == false)
		glEnable(GL_DEPTH_TEST);
	else if (state == true)
		glDisable(GL_DEPTH_TEST);
}
void ModuleRenderer3D::SetCullface(bool state) {
	if (state == false)
		glEnable(GL_CULL_FACE);
	else if (state == true)
		glDisable(GL_CULL_FACE);
}
void ModuleRenderer3D::SetLighting(bool state) {
	if (state == false)
		glEnable(GL_LIGHTING);
	else if (state == true)
		glDisable(GL_LIGHTING);
}
void ModuleRenderer3D::SetColormaterial(bool state) {
	if (state == false)
		glEnable(GL_COLOR_MATERIAL);
	else if (state == true)
		glDisable(GL_COLOR_MATERIAL);
}
void ModuleRenderer3D::SetTexture2D(bool state) {
	if (state == false)
		glEnable(GL_TEXTURE_2D);
	else if (state == true)
		glDisable(GL_TEXTURE_2D);
}
void ModuleRenderer3D::SetCubemap(bool state) {
	if (state == false)
		glEnable(GL_TEXTURE_CUBE_MAP);
	else if (state == true)
		glDisable(GL_TEXTURE_CUBE_MAP);
}
void ModuleRenderer3D::SetPolygonssmooth(bool state) {
	if (state == false)
		glEnable(GL_POLYGON_SMOOTH);
	else if (state == true)
		glDisable(GL_POLYGON_SMOOTH);
}