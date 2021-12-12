#pragma once
#include "Globals.h"
#include <vector>

class GameObject;

class ModuleComponent
{
public:
	enum class ComponentType {
	Mesh,
	Transform,
	Material,
	Camera,
	None
	};
	ModuleComponent(ComponentType type);
	ModuleComponent(ComponentType type, GameObject* owner);
	~ModuleComponent();

	
	virtual void Update() = 0;
	virtual void CleanUp() = 0;
	virtual void DrawInspector() = 0;

	void Disable();
	void Enable();

	bool IsActive();

	inline ComponentType GetType() const { return type; };

	ComponentType ReturnType();
	GameObject* ReturnGameObject();


public:

	bool active = true;

	GameObject* owner = nullptr;
	ComponentType type = ComponentType::None;
};