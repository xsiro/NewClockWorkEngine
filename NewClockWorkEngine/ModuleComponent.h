#pragma once
#include "Globals.h"
#include <vector>

class GameObject;

enum class ComponentType {
	Mesh,
	Transform,
	Material
};

class ModuleComponent
{
public:

	ModuleComponent(ComponentType type);
	ModuleComponent(ComponentType type, GameObject* owner);
	~ModuleComponent();

	virtual void Enable();
	virtual void Update() = 0;
	virtual void CleanUp() = 0;
	virtual void Disable();
	virtual void DrawInspector() = 0;
	bool IsActive();
	ComponentType ReturnType();
	GameObject* ReturnGameObject();


public:

	bool active = true;
	GameObject* owner = nullptr;
	ComponentType type;


};