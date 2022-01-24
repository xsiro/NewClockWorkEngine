#pragma once
#include "Module.h"
#include "Globals.h"

#define PRIMITIVES_IN_SCENE 10

class mat4x4;
class GameObject;
enum class MeshDrawMode;

class ModuleSceneIntroIntro : public Module
{
public:
	ModuleSceneIntroIntro(bool start_enabled = true);
	~ModuleSceneIntroIntro();

	bool Init();
	bool GameInit();
	bool Start();
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status GameUpdate(float gameDt)override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	bool SetSelectedGameObject(GameObject* selected, bool addMode = false);
	bool RemoveGameObjFromSelected(GameObject* toRemove);

	std::vector<GameObject*> GetSelectedGameObject();
	bool UpdateInfoOnSelectedGameObject();

	void AddObjName(std::string& name); //add a name to the name vector
	void RemoveName(std::string name);//removes a name from the name vector
	void ChangeObjName(std::string oldName, std::string& newName); //modifies a name from the vector

	void TestRayHitObj(LineSegment ray);

private:
	int DoesNameExist(std::string name); //returns an index of where the current name is, defaults in -1
	void MakeNameUnique(std::string& name); //if this object is going to have the same name as another, make it unique

public:
	GameObject* root; 

	std::vector<GameObject*> selectedGameObjs;

	MeshDrawMode maxSceneDrawMode;
	
	bool mouseActive;//checks whether the 3d scene can be interacted with the mouse (when hovering or mantaining click after hover)

private:
	std::vector<std::string> objNames;
};