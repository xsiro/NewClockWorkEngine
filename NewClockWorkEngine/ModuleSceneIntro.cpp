#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleGui.h"
#include "imgui.h"
#include "ModuleMesh.h"
#include "ModuleMaterial.h"
#include "GameObject.h"
#include "FileSystem.h"

class ModuleMesh;

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled), selected(nullptr)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	GameObject* house = App->importer->LoadFBX("Assets/BakerHouse.fbx");
	App->importer->LoadTexture("Assets/Baker_house.png");
	CreateGameObject(house);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	for (size_t i = 0; i < game_objects.size(); i++)
	{
		delete game_objects[i];
		game_objects[i] = nullptr;
	}
	game_objects.clear();

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	Cube cube(1.0f, 1.0f, 1.0f);
	if (App->gui->GetWireframe() == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (App->gui->GetWireframe() == false) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (App->gui->cube) {
		ModuleMesh* Cube = new ModuleMesh();
		Cube->CreateCubeDirect();
	}

	if (App->gui->pyramid)
	{
		ModuleMesh* Pyramid = new ModuleMesh();
		Pyramid->CreatePyramid();
	}

	if (App->gui->cylinder)
	{
		ModuleMesh* Cylinder = new ModuleMesh();
		Cylinder->CreateCylinder(1, 3, 6);
	}

	if (App->gui->sphere)
	{
		ModuleMesh* Sphere = new ModuleMesh();
		Sphere->CreateSphere(1, 24, 24);
	}

	for (size_t i = 0; i < game_objects.size(); i++)
	{
		game_objects[i]->Update();
	}

	return UPDATE_CONTINUE;
}

GameObject* ModuleSceneIntro::CreateGameObject(GameObject* father) {

	GameObject* newgo = new GameObject();
	newgo->parent = father;
	game_objects.push_back(newgo);

	selected = father;

	return newgo;
}




