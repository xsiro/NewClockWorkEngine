#include "Globals.h"
#include <vector>
#include <iostream>
#include <string>
#include "Imgui/include/imgui.h"

class ModuleComponent;
class ModuleTransform;
class ModuleMaterial;
enum class ComponentType;

class GameObject
{
public:
	GameObject(char* name);
	GameObject(GameObject* parent, char* name);
	~GameObject();

	void Update();
	const char* GetName();
	ModuleComponent* GetComponent(ComponentType component);
	ModuleComponent* AddComponent(ModuleComponent* component);
	void DeleteComponent(ComponentType type);
	std::vector<ModuleComponent*> GetComponents()const;
	bool HasComponentType(ComponentType type);

public:
	ModuleTransform* transform = nullptr;
	ModuleMaterial* material = nullptr;
	GameObject* parent;
	bool active;
	std::string name;
	std::vector<ModuleComponent*> components;
	std::vector<GameObject*> children;
};