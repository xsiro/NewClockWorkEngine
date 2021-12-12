#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "imgui/include/imgui.h"
#include <vector>

class GameObject;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt) override;
	bool CleanUp();

	void SetSelectedObject(GameObject* object);

	bool drawBB = false;

	GameObject* CreateGameObject(char* name, char* meshPath = "", char* texturePath = "", bool isRoot = false);
	GameObject* rootObject;
	GameObject* selected = nullptr;

	std::vector<GameObject*> game_objects;

	
};
