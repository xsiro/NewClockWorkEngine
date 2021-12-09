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
#include "ModuleImporter.h"


class ModuleMesh;

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
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

	CreateGameObject("BakerHouse", "Assets/BakerHouse.fbx", "Assets/Baker_house.png");

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	std::vector<GameObject*>::iterator item = game_objects.begin();
	for (; item != game_objects.end(); ++item)
	{
		(*item)->CleanUp();
		delete (*item);
	}

	game_objects.clear();

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	Planes p(vec3(0, 1, 0));
	p.axis = true;
	p.Render();

	Cube cube(1.0f, 1.0f, 1.0f);


	std::vector<GameObject*>::iterator item = game_objects.begin();
	for (; item != game_objects.end(); ++item)
	{
		(*item)->Update();
		if ((*item)->GetName() != "root")
		{
			((GameObject*)*item)->DrawBB(drawBB);
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::CreateGameObject(char* name, char* meshPath = "", char* texturePath = "")
{
	GameObject* newGameObject = nullptr;
	if (meshPath != "")
	{
		std::vector<Mesh*> meshes = Importer::MeshImporter::Import(meshPath);

		if (meshes.size() == 0)
		{
			LOG("(ERROR) No meshes found in %s", meshPath);
			return;
		}

		newGameObject = new GameObject(nullptr, name);
		game_objects.push_back(newGameObject);

		if (meshes.size() == 1)
		{
			newGameObject->AddComponent(new ModuleMesh(newGameObject, meshPath, meshes.front()));
			if (texturePath != "")
				newGameObject->AddComponent(new ModuleMaterial(game_objects.back(), texturePath, Importer::TextureImp::Import(texturePath)));
		}
		else
		{
			std::vector<Mesh*>::iterator item = meshes.begin();
			for (; item != meshes.end(); ++item)
			{
				GameObject* childGameObject = new GameObject(newGameObject, name);
				ModuleMesh* newComp = new ModuleMesh(childGameObject, meshPath, (*item));

				childGameObject->AddComponent((ModuleComponent*)newComp);

				if (texturePath != "")
					childGameObject->AddComponent(new ModuleMaterial(game_objects.back(), texturePath, Importer::TextureImp::Import(texturePath)));

				newGameObject->children.push_back(childGameObject);
			}
		}
	}
}

void ModuleSceneIntro::SetSelectedObject(GameObject* object)
{
	if (object != nullptr)
	{
		selected = object;
		LOG("Object selected: %s", selected->GetName());
	}
}


