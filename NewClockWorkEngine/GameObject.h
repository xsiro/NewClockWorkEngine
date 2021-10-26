#include "Globals.h"
#include <vector>
#include <iostream>

class ModuleComponent;
enum class ComponentType;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();

	ModuleComponent* GetComponent(ComponentType component);
	ModuleComponent* CreateComponent(ComponentType type);
	void AddComponent(ModuleComponent* component);

public:

	bool active;
	std::string name;
	std::vector<ModuleComponent*> components;
};
