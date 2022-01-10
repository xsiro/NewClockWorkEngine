#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "ModuleImporter.h"
#include "ModuleMesh.h"
#include "glmath.h"
#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Brofiler/Brofiler.h"
#include "ModuleGui.h"
#include "ModuleComponent.h"
#include "ModuleMaterial.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "ModuleSceneIntro.h"
#include "ModuleTransform.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include <Windows.h>
#include "Gizmos.h"



#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_sdl.h"
#include "imgui/include/imgui_impl_opengl3.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include <vector>

#pragma comment (lib, "glu32.lib")   
#pragma comment (lib, "opengl32.lib") 
#pragma comment (lib, "glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled), context()
{
	SetCullface = true;
	SetColormaterial = true;
	SetLighting = true;
	SetDepthtest = true;
	SetTexture2D = true;
	wireframeMode = false;
	checkersId = 0;
}

ModuleRenderer3D::~ModuleRenderer3D()
{

}

bool ModuleRenderer3D::Init()
{
	LOG("(INIT) Creating 3D Renderer context");
	bool ret = true;
	
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("(ERROR) OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{

		if (glewInit() != GLEW_OK) {
			LOG("(ERROR) ERROR ON GLEWINIT");
			ret = false;
		}
		else
			LOG("(INIT) Glew initialized succesfully!");

		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("(ERROR) Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("(ERROR) Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		glClearColor(0.f, 0.f, 0.f, 1.f);

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("(ERROR) Error initializing OpenGL! %s\n", gluErrorString(error));
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

		glEnable(GL_LINE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		lights[0].Active(true);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	CreateChekerTexture();
	Importer::TextureImp::InitDevil();


	return ret;
}

update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(App->camera->currentCamera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->currentCamera->GetPos().x, App->camera->currentCamera->GetPos().y, App->camera->currentCamera->GetPos().z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::PostUpdate(float dt)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf((GLfloat*)&App->camera->currentCamera->frustum.ProjectionMatrix().Transposed());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);

	App->gui->Draw();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}


bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);
	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	if (App->camera->currentCamera != nullptr)
		App->camera->currentCamera->SetAspectRatio(((float)width / (float)height));
}


void ModuleRenderer3D::DrawMesh(ResourceMesh* mesh, float4x4 transform, ResourceMaterial* material, bool drawVertexNormals, bool drawBoundingBox, GameObject* gameObject)
{
	glStencilMask(0x00);

	bool doStencil = false;
	if (gameObject == App->scene_intro->selected && gameObject != nullptr) //Stencil selection
	{
		doStencil = true;
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	if (!IsObjectInScreen(gameObject) && App->camera->currentCamera->cull)
		return;

	wireframeMode == false ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : (glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

	glPushMatrix();
	glMultMatrixf((float*)&transform.Transposed());

	glLineWidth(2);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Pass TextureID
	if (!wireframeMode)
	{
		if (material != nullptr)
		{
			if (material->GetId() == 0)
				glBindTexture(GL_TEXTURE_2D, checkersId);
			else
				glBindTexture(GL_TEXTURE_2D, material->GetId());
		}
		else
		{

		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffersId[ResourceMesh::texture]);
	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffersId[ResourceMesh::normal]);
	glNormalPointer(GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffersId[ResourceMesh::vertex]);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffersId[ResourceMesh::index]);
	glDrawElements(GL_TRIANGLES, mesh->buffersSize[ResourceMesh::index], GL_UNSIGNED_INT, nullptr);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (drawVertexNormals)
	{
		DrawVertexNormals(mesh,transform);
	}

	if (drawBoundingBox || App->scene_intro->drawBB)
	{
		if (gameObject != nullptr)

			if (gameObject->HasComponentType(ModuleComponent::ComponentType::Mesh))
			{
				vec* corners = new vec[8];
				glColor4f(0.5, 0, 0.5, 1);
				gameObject->obb.GetCornerPoints(corners);
				DrawBox(corners);
				glColor4f(0, 0.5, 0, 1);
				gameObject->aabb.GetCornerPoints(corners);
				DrawBox(corners);
				RELEASE_ARRAY(corners);
			}
	}

}

void ModuleRenderer3D::DrawVertexNormals(ResourceMesh* mesh, float4x4 transform)
{
	//Draw Normals
	glPushMatrix();
	glMultMatrixf((float*)&transform.Transposed());
	glBegin(GL_LINES);
	uint loops = mesh->buffersSize[ResourceMesh::vertex];
	for (uint i = 0; i < loops; i += 3)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glVertex3f(mesh->vertices[i], mesh->vertices[i + 1], mesh->vertices[i + 2]);
		glVertex3f(mesh->vertices[i] + mesh->normals[i], mesh->vertices[i + 1] + mesh->normals[i + 1], mesh->vertices[i + 2] + mesh->normals[i + 2]);

	}
	glPopMatrix();
	glEnd();
}

void ModuleRenderer3D::GenerateBuffers(ResourceMesh* newMesh)
{
	//Vertex buffer
	glGenBuffers(1, (GLuint*)&(newMesh->buffersId[ResourceMesh::vertex]));
	glBindBuffer(GL_ARRAY_BUFFER, newMesh->buffersId[ResourceMesh::vertex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * newMesh->buffersSize[ResourceMesh::vertex] * 3, newMesh->vertices, GL_STATIC_DRAW);


	if (newMesh->indices != nullptr)
	{
		//Index buffer
		glGenBuffers(1, (GLuint*)&(newMesh->buffersId[ResourceMesh::index]));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh->buffersId[ResourceMesh::index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * newMesh->buffersSize[ResourceMesh::index], newMesh->indices, GL_STATIC_DRAW);
	}

	if (newMesh->normals != nullptr)
	{
		//Normals buffer
		glGenBuffers(1, (GLuint*)&(newMesh->buffersId[ResourceMesh::normal]));
		glBindBuffer(GL_ARRAY_BUFFER, newMesh->buffersId[ResourceMesh::normal]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * newMesh->buffersSize[ResourceMesh::normal] * 3, newMesh->normals, GL_STATIC_DRAW);
	}

	if (newMesh->textureCoords != nullptr)
	{
		glGenBuffers(1, (GLuint*)&newMesh->buffersId[ResourceMesh::texture]);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh->buffersId[ResourceMesh::texture]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * newMesh->buffersSize[ResourceMesh::texture] * 2, newMesh->textureCoords, GL_STATIC_DRAW);
	}

}

void ModuleRenderer3D::CreateChekerTexture()
{
	GLubyte checkerImage[64][64][4];
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;

			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	checkersId = Importer::TextureImp::CreateTexture(checkerImage, 64, 64, GL_RGBA);

}

void ModuleRenderer3D::SwitchCullFace()
{
	glIsEnabled(GL_CULL_FACE) == false ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void ModuleRenderer3D::SwitchDepthTest()
{
	if (SetDepthtest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void ModuleRenderer3D::SwitchLighting()
{
	if (SetLighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
}

void ModuleRenderer3D::SwitchTexture2d()
{
	if (SetTexture2D)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void ModuleRenderer3D::SwitchColorMaterial()
{
	if (glColorMaterial)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);
}

void ModuleRenderer3D::DrawScenePlane(int size)
{
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	for (int i = -size; i <= size; i++)
	{
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(size, 0, i);
		glVertex3d(-size, 0, i);
	}
	glEnd();
}

void ModuleRenderer3D::DrawLine(float3 a, float3 b)
{
	glDisable(GL_LIGHTING);
	glColor4f(0, 1, 0, 1);
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	glVertex3d(a.x, a.y, a.z);
	glVertex3d(b.x, b.y, b.z);

	glEnd();
	glColor4f(1, 1, 1, 1);
	glLineWidth(1.0f);
	SwitchLighting();
}

void ModuleRenderer3D::DrawBox(float3* corners)
{
	glDisable(GL_LIGHTING);
	

	glBegin(GL_LINES);

	//Between-planes right
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[3]);

	//Between-planes left
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);
	glVertex3fv((GLfloat*)&corners[6]);

	//Far plane horizontal
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[7]);

	//Near plane horizontal
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Near plane vertical
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);

	//Far plane vertical
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);

	glEnd();
	glColor4f(1, 1, 1, 1);

	SwitchLighting();
}

bool ModuleRenderer3D::IsObjectInScreen(GameObject* gameObject)
{
	float3 corners[8];
	gameObject->aabb.GetCornerPoints(corners); // get the corners of the box into the corners array

	for (int plane = 0; plane < 6; ++plane) {

		int iInCount = 8;
		int iPtIn = 1;

		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (App->camera->cullingCamera->planes[plane].IsOnPositiveSide(corners[i])) { //<-- “IsOnPositiveSide” from MathGeoLib
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if (iInCount == 0)
			return(false);
	}
	// we must be partly in then otherwise
	return(true);
}