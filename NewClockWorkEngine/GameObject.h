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
	const char* GetName();
	void SetName(const char* name);
	void Enable();
	void Disable();
	bool IsActive();
	void DrawBB(bool drawBB);
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
	void DeleteComponent(ModuleComponent::ComponentType type);
	std::vector<ModuleComponent*> GetComponents()const;
	bool HasComponentType(ModuleComponent::ComponentType type);
	void UpdateBoundingBoxes();
	void UpdatedTransform();


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