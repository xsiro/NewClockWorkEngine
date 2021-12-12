#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "imgui/include/imgui.h"
#include <vector>

class GameObject;
class ConfigNode;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt) override;
	bool CleanUp();

	void SetSelectedObject(GameObject* object);
	void SaveScene();

	bool drawBB = false;

	GameObject* CreateGameObject(char* name, GameObject* parent = nullptr, bool isRoot = false);
	uint GetNameRepeats(const char* name);
	GameObject* rootObject;
	GameObject* selected = nullptr;

	std::vector<GameObject*> game_objects;

	
};
