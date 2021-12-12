#include "Globals.h"
#include <vector>
#include <iostream>
#include <string>
#include "Imgui/include/imgui.h"
#include "MathGeoLib/src/MathGeoLib.h"

#include "ModuleComponent.h"

class ModuleTransform;
class ModuleMaterial;
class ModuleMesh;
enum class ComponentType;

class GameObject
{
public:
	GameObject(const char* name);
	GameObject(GameObject* parent, const char* name);
	~GameObject();

	void Update();
	void CleanUp();

	void SetName(const char* name);
	void Enable();
	void Disable();

	void UpdateBoundingBoxes();
	void UpdatedTransform();
	void DeleteComponent(ModuleComponent::ComponentType type);

	bool IsActive();
	bool HasComponentType(ModuleComponent::ComponentType type);

	const char* GetName();
	template<typename CTemplate>
	const CTemplate* GetComponent() const
	{
		ModuleComponent::ComponentType type = CTemplate::GetType();
		for (int i = 0; i < components.size(); i++)
		{
			if (type == components[i]->GetType())
			{
				return ((CTemplate*)components[i]);
			}
		}
		return nullptr;
	}
	ModuleComponent* GetComponent(ModuleComponent::ComponentType component);
	ModuleComponent* AddComponent(ModuleComponent* component);
	ModuleMesh* GetComponentMesh();
	ModuleTransform* GetComponentTransform();
	
	std::vector<ModuleComponent*> GetComponents()const;
	
private:	
	bool active;
	std::string name;
	std::vector<ModuleComponent*> components;

public:

	ModuleTransform* transform = nullptr;
	ModuleMaterial* material = nullptr;

	GameObject* parent;
	std::vector<GameObject*> children;

	OBB obb;
	AABB aabb;
};