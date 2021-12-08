#include "Globals.h"
#include <vector>
#include <iostream>
#include <string>
#include "Imgui/include/imgui.h"

class ModuleComponent;
class ModuleTransform;
class ModuleMaterial;
class ModuleMesh;
enum class ComponentType;

class GameObject
{
public:
	GameObject(char* name);
	GameObject(GameObject* parent, char* name);
	~GameObject();

	void Update();
	void CleanUp();
	const char* GetName();
	void Enable();
	void Disable();
	bool IsActive();
	ModuleComponent* GetComponent(ComponentType component);
	ModuleComponent* AddComponent(ModuleComponent* component);
	void DeleteComponent(ComponentType type);
	std::vector<ModuleComponent*> GetComponents()const;
	bool HasComponentType(ComponentType type);


private:	
	bool active;
	std::string name;
	std::vector<ModuleComponent*> components;

public:
	ModuleTransform* transform = nullptr;
	ModuleMaterial* material = nullptr;
	GameObject* parent;
	std::vector<GameObject*> children;
};