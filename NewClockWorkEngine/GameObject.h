#include "Globals.h"
#include <vector>
#include <iostream>
#include <string>
#include "Imgui/include/imgui.h"
#include "MathGeoLib/src/MathGeoLib.h"

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
	void DrawBB(bool drawBB);
	template<typename CTemplate>
	const CTemplate* GetComponent() const
	{
		ComponentType type = CTemplate::GetType();
		for (int i = 0; i < components.size(); i++)
		{
			if (type == components[i]->GetType())
			{
				return ((CTemplate*)components[i]);
			}
		}
		return nullptr;
	}
	ModuleComponent* GetComponent(ComponentType component);
	ModuleComponent* AddComponent(ModuleComponent* component);
	ModuleMesh* GetComponentMesh();
	ModuleTransform* GetComponentTransform();
	void DeleteComponent(ComponentType type);
	std::vector<ModuleComponent*> GetComponents()const;
	bool HasComponentType(ComponentType type);
	void UpdateBoundingBoxes();


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