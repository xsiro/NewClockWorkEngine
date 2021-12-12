#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleGui.h"
#include "imgui.h"
#include "ModuleMesh.h"
#include "ModuleTransform.h"
#include "ModuleMaterial.h"
#include "ModuleCamera.h"
#include "ModuleCamera3D.h"
#include "GameObject.h"
#include "FileSystem.h"
#include "ModuleImporter.h"
#include "Config.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{
	rootObject = CreateGameObject("rootObject", nullptr, true);
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->SetPosition(float3(0, 5, -5));

	Importer::SceneImporter::Import("Assets/street/Street environment_V01.FBX");
	return ret;
}

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

update_status ModuleSceneIntro::Update(float dt)
{
	App->renderer3D->DrawScenePlane(200);
	//rootObject->Update();

	std::vector<GameObject*>::iterator item = game_objects.begin();
	for (; item != game_objects.end(); ++item)
	{
		(*item)->Update();

	}

	return UPDATE_CONTINUE;
}

GameObject* ModuleSceneIntro::CreateGameObject(char* name, GameObject* parent, bool isRoot)
{
	GameObject* newGameObject;
	uint repeats = GetNameRepeats(name);

	if (isRoot)
	{
		newGameObject = new GameObject(nullptr, "Root Object");
	}
	else
	{

		std::string finalName = name;
		if (repeats > 0)
			finalName += "(" + std::to_string(repeats) + ")";

		newGameObject = new GameObject(parent, finalName.c_str());

		if (strcmp(newGameObject->GetName(), "aniTest") == 0)
		{
			newGameObject->transform->SetPosition(float3(0.1, 0.1, 0.1));
		}
	}

	game_objects.push_back(newGameObject);

	return newGameObject;
}

uint ModuleSceneIntro::GetNameRepeats(const char* name)
{
	uint repeats = 0;
	for (std::vector<GameObject*>::iterator item = game_objects.begin(); item != game_objects.end(); item++)
	{
		//get rid of the (n)
		std::string str2;
		std::string str = (*item)->GetName();
		if (str.find("(") == std::string::npos)
		{
			str2 = str;
		}
		else
		{
			str2 = str.substr(0, str.find_last_of("("));
		}

		if (strcmp(name, str2.c_str()) == 0)
			repeats += 1;
	}
	return repeats;
}

void ModuleSceneIntro::SetSelectedObject(GameObject* object)
{
	if (object != nullptr)
	{
		selected = object;
		LOG("Object selected: %s", selected->GetName());
	}
}

void ModuleSceneIntro::SaveScene()
{
	ConfigNode* sceneNode;

	uint64 id = Importer::SceneImporter::SaveScene(sceneNode, game_objects);

	std::string path = "Library/Scenes/";
	std::string idString = std::to_string(id);
	path += idString + ".scene";

	char* buffer;
	uint size = sceneNode->Serialize(&buffer); 
	App->filesystem->Save(path.c_str(), buffer, size);
}