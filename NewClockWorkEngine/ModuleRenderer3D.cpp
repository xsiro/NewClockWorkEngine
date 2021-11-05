#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "ModuleImporter.h"
#include "glmath.h"
#include "Glew/include/glew.h"
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

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

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

		/*mesh = &App->importer->myMesh;

		glGenBuffers(1, (GLuint*)&mesh->id_vertex);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, mesh->vertex, GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&mesh->id_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_index, mesh->index, GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&mesh->id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * mesh->num_normals * 3, mesh->normals, GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&mesh->id_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_texcoords * 2, mesh->texcoords, GL_STATIC_DRAW);

		LoadTextures();*/
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

	//Color c = App->camera->background;
	//glClearColor(c.r, c.g, c.b, c.a);

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
	App->gui->Draw();
	/*App->gui->PostUpdate(dt);*/
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
//
//void ModuleRenderer3D::LoadFBXBuffer() {
//
//	mesh = &App->importer->myMesh;
//
//	glGenBuffers(1, (GLuint*)&mesh->id_vertex);
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, mesh->vertex, GL_STATIC_DRAW);
//
//	glGenBuffers(1, (GLuint*)&mesh->id_index);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_index, mesh->index, GL_STATIC_DRAW);
//
//	glGenBuffers(1, (GLuint*)&mesh->id_normals);
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * mesh->num_normals * 3, mesh->normals, GL_STATIC_DRAW);
//
//	glGenBuffers(1, (GLuint*)&mesh->id_texcoords);
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_texcoords);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_texcoords * 2, mesh->texcoords, GL_STATIC_DRAW);
//
//}

//void ModuleRenderer3D::RenderFBX() {
//	if (!App->gui->check)
//	{
//		if (rendered)
//		{
//			App->importer->LoadTexture("Assets/Baker_house.png");
//			rendered = true;
//
//		}
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, mesh->image_id);
//		glBindTexture(GL_TEXTURE_2D, App->importer->Gl_Tex);
//	}
//	else if (App->gui->check)
//	{
//		rendered = false;
//
//		glEnable(GL_TEXTURE_2D);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
//			0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
//
//	}
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
//	glVertexPointer(3, GL_FLOAT, 0, NULL);
//
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
//	glNormalPointer(GL_FLOAT, 0, NULL);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
//
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_texcoords);
//	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
//
//	glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_NORMAL_ARRAY, 0);
//
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisable(GL_TEXTURE_2D);
//}
//
//void ModuleRenderer3D::DrawVertexNormalLines()
//{
//
//	glBegin(GL_LINES);
//	glColor3f(0.0f, 0.0f, 1.5f);
//
//	for (size_t i = 0; i < App->importer->myMesh.num_vertex * 3; i += 3)
//	{
//		float v_x = App->importer->myMesh.vertex[i];
//		float v_y = App->importer->myMesh.vertex[i + 1];
//		float v_z = App->importer->myMesh.vertex[i + 2];
//
//		float n_x = App->importer->myMesh.normals[i];
//		float n_y = App->importer->myMesh.normals[i + 1];
//		float n_z = App->importer->myMesh.normals[i + 2];
//
//		glVertex3f(v_x, v_y, v_z);
//		glVertex3f(v_x + n_x, v_y + n_y, v_z + n_z);
//	}
//
//	glEnd();
//}
//
//void ModuleRenderer3D::DrawFaceNormalLines() {
//
//	glBegin(GL_LINES);
//	glColor3f(0.0f, 1.5f, 0.0f);
//
//	for (size_t i = 0; i < App->importer->myMesh.num_vertex * 3; i += 3)
//	{
//		float x = (App->importer->myMesh.vertex[i] + App->importer->myMesh.vertex[i + 3] + App->importer->myMesh.vertex[i + 6]) / 3;
//		float y = (App->importer->myMesh.vertex[i + 1] + App->importer->myMesh.vertex[i + 4] + App->importer->myMesh.vertex[i + 7]) / 3;
//		float z = (App->importer->myMesh.vertex[i + 2] + App->importer->myMesh.vertex[i + 5] + App->importer->myMesh.vertex[i + 8]) / 3;
//
//		float nx = App->importer->myMesh.normals[i];
//		float ny = App->importer->myMesh.normals[i + 1];
//		float nz = App->importer->myMesh.normals[i + 2];
//
//		glVertex3f(x, y, z);
//		glVertex3f(x + nx, y + ny, z + nz);
//	}
//	glEnd();
//
//}
//
//void ModuleRenderer3D::LoadTextures() {
//
//	for (int i = 0; i < 64; i++) {
//		for (int j = 0; j < 64; j++) {
//			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
//			checkImage[i][j][0] = (GLubyte)c;
//			checkImage[i][j][1] = (GLubyte)c;
//			checkImage[i][j][2] = (GLubyte)c;
//			checkImage[i][j][3] = (GLubyte)255;
//		}
//	}
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glGenTextures(1, &texture_id);
//	glBindTexture(GL_TEXTURE_2D, texture_id);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
//		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
//
//}

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

void ModuleRenderer3D::SetDepthtest(bool state)
{
	if (state == false)
		glEnable(GL_DEPTH_TEST);
	else if (state == true)
		glDisable(GL_DEPTH_TEST);
}
void ModuleRenderer3D::SetCullface(bool state)
{
	if (state == false)
		glEnable(GL_CULL_FACE);
	else if (state == true)
		glDisable(GL_CULL_FACE);
}
void ModuleRenderer3D::SetLighting(bool state)
{
	if (state == false)
		glEnable(GL_LIGHTING);
	else if (state == true)
		glDisable(GL_LIGHTING);
}
void ModuleRenderer3D::SetColormaterial(bool state)
{
	if (state == false)
		glEnable(GL_COLOR_MATERIAL);
	else if (state == true)
		glDisable(GL_COLOR_MATERIAL);
}
void ModuleRenderer3D::SetTexture2D(bool state)
{
	if (state == false)
		glEnable(GL_TEXTURE_2D);
	else if (state == true)
		glDisable(GL_TEXTURE_2D);
}
void ModuleRenderer3D::CreateCube()
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 10.f, 0.f);
	glEnd();
	glLineWidth(1.0f);
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
