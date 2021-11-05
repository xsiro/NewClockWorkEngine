#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleGui.h"
#include "imgui.h"
#include "ModuleMesh.h"
#include "FileSystem.h"
#include "GameObject.h"

class ComponentMesh;

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
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
	
	//walls colliders
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

// Update


update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	



	Cube cube(1.0f, 1.0f, 1.0f);
	if (App->gui->wireframe == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (App->gui->wireframe == false) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (App->gui->cube) {
		ComponentMesh CreateCubeDirect();
	}

	if (App->gui->pyramid)
	{
		ComponentMesh CreatePyramid();
	}

	if (App->gui->cylinder)
	{
		ComponentMesh CreateCylinder();
	}

	if (App->gui->sphere)
	{
		ComponentMesh CreateSphere();
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


