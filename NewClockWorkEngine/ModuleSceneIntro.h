#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "imgui/include/imgui.h"

class GameObject;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt) override;


	bool CleanUp();
	GameObject* CreateGameObject(GameObject* GameObject);


	GameObject* selected;
	std::vector<GameObject*> game_objects;
};
