#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include <vector>


#include "MathGeoLib/include/MathGeoLib.h"

class Component;
class ModuleTransform;
enum class ComponentType;

class GameObject
{
public:
	unsigned int ID; //save / load func

	GameObject(GameObject* parent, std::string name, float4x4 transform, bool showAABB = true, bool isLocalTrans = true);
	
	void Awake();
	void GameInit();

	void Update(float dt);
	void GameUpdate(float gameDt);

	~GameObject();



	void RemoveChildren(GameObject* toRemove);

	void ChangeParent(GameObject* newParent);

	void RemoveMyselfFromParent();

	Component* CreateComponent(ComponentType type,unsigned int compID=0);

	std::string GetName();

	bool IsParentActive();

	GameObject* parent;

	template<typename Comp>
	Comp* GetComponent()
	{
		for (int i = 0; i < components.size(); i++) { Comp* c = dynamic_cast<Comp*>(components[i]);    if (c != nullptr)    return    c; }
		return nullptr;
	}

	template<typename Comp>
	std::vector<Comp*> GetComponents()
	{
		std::vector<Comp*> compVec;
		for (int i = 0; i < components.size(); i++) { Comp* c = dynamic_cast<Comp*>(components[i]);    if (c != nullptr)   compVec.push_back(c); }
		return compVec;
	}

	void GetChildWithID(unsigned int ID, GameObject*& childOut);

	void UpdateChildTransforms();
	void UpdateBoundingBox();
	void GetObjAndAllChilds(std::vector<GameObject*>&childs);
	AABB GetWorldAABB()const;
	void DrawOnEditorAllComponents();
	std::vector<Component*> GetAllComponents();
private:
	void DrawGameObject();
	void GetPointsFromAABB(AABB&aabb,std::vector<float3>& emptyVector);
private:
	std::vector<Component*> components;
	ModuleTransform* transform;



	std::string name;

	AABB globalAABB;
	OBB globalOBB;

public:
	bool isActive;
	bool focused;
	bool selected;
	bool displayBoundingBox;
	bool bbHasToUpdate;
	std::vector<GameObject*> children; //we need them public for hierarchy

	static int numberOfObjects;
};



#endif // !__GAME_OBJECT_H__
