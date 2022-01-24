#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ResourceMesh.h"


#include <map>
#include "ModuleTransform.h"
#include "ModuleMesh.h"


ModuleSceneIntroIntro::ModuleSceneIntroIntro(bool start_enabled) :Module(start_enabled)
{
	maxSceneDrawMode = MeshDrawMode::DRAW_MODE_BOTH;
	mouseActive = true;
	root = nullptr;
}

ModuleSceneIntroIntro::~ModuleSceneIntroIntro()
{

}

bool ModuleSceneIntroIntro::Init()
{
	bool ret = true;
	return ret;
}

bool ModuleSceneIntroIntro::GameInit()
{
	if (root)
		root->GameInit();
	return true;
}

bool ModuleSceneIntroIntro::Start()
{
	bool ret = true;

	root = new GameObject(nullptr, "SceneRoot", float4x4::identity, false);
	App->camera->Move(float3(1.0f, 1.0f, 0.0f));

	return ret;
}

update_status ModuleSceneIntroIntro::PreUpdate(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		while (!selectedGameObjs.empty())
		{
			GameObject* aux = selectedGameObjs.back();
			selectedGameObjs.pop_back();
			delete aux;
			aux = nullptr;
		}

		/*if (selectedGameObjs.size() > 0)
		{
			GameObject* aux = selectedGameObjs.back();
			selectedGameObjs.pop_back();
			delete aux;
		}*/
	}
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntroIntro::Update(float dt)
{

	if (root)
		root->Update(dt);

	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntroIntro::GameUpdate(float gameDt)
{

	if (root)
		root->GameUpdate(gameDt);

	return UPDATE_CONTINUE;
}


update_status ModuleSceneIntroIntro::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleSceneIntroIntro::CleanUp()
{
	bool ret = true;
	objNames.clear();

	selectedGameObjs.clear();

	if (root)
		delete root;

	root = nullptr;

	return ret;
}

bool ModuleSceneIntroIntro::SetSelectedGameObject(GameObject* selected, bool addMode)
{
	bool ret = false;
	if (selected != root)
	{

		if (!addMode)
		{
			for (int i = 0; i < selectedGameObjs.size(); i++)
			{
				if (selectedGameObjs[i] != nullptr)
				{
					selectedGameObjs[i]->focused = false;
					selectedGameObjs[i]->selected = false;
				}
			}
			selectedGameObjs.clear();

		}

		if (selected != nullptr)
		{
			RemoveGameObjFromSelected(selected);

			for (int i = 0; i < selectedGameObjs.size(); i++)
			{
				selectedGameObjs[i]->focused = false;
			}
			selectedGameObjs.push_back(selected);
			selected->focused = true;
			selected->selected = true;
			ret = true;
		}
	}
	return ret;
}

//if the item exists in the Game Object vector, this method erases it from the list
bool ModuleSceneIntroIntro::RemoveGameObjFromSelected(GameObject* toRemove)
{
	bool ret = false;
	bool wasFocused = false;
	for (int i = 0; i < selectedGameObjs.size(); i++)
	{
		if (selectedGameObjs[i] == toRemove)
		{
			if (selectedGameObjs[i]->focused)
			{
				selectedGameObjs[i]->focused = false;
				wasFocused = true;
			}
			selectedGameObjs[i]->selected = false;

			selectedGameObjs[i] = nullptr;
			selectedGameObjs.erase(selectedGameObjs.begin() + i);
			ret = true;
			break;
		}
	}

	if (wasFocused)
	{
		for (int i = selectedGameObjs.size()-1; i >=0; i--)//focus on the last selected object
		{
			if (selectedGameObjs[i]->selected)
			{
				selectedGameObjs[i]->focused = true;
				break;
			}
		}
	}

	return ret;
}

std::vector<GameObject*> ModuleSceneIntroIntro::GetSelectedGameObject()
{
	return selectedGameObjs;
}

bool ModuleSceneIntroIntro::UpdateInfoOnSelectedGameObject()
{
	bool ret = true;
	if (selectedGameObjs.size() > 0)
	{
		selectedGameObjs.back()->DrawOnEditorAllComponents();
	}
	return ret;
}

void ModuleSceneIntroIntro::AddObjName(std::string& name)
{

	int index = DoesNameExist(name);
	if (index == -1)
	{
		objNames.push_back(name);
	}
	else
	{
		MakeNameUnique(name);
		objNames.push_back(name);
	}
}

void ModuleSceneIntroIntro::RemoveName(std::string name)
{
	int index = DoesNameExist(name);
	if (index != -1)
	{
		objNames.erase(objNames.begin() + index);
	}
}

void ModuleSceneIntroIntro::ChangeObjName(std::string oldName, std::string& newName)
{
	int index = DoesNameExist(oldName);
	if (index == -1)//if oldName doesn't exist push it to the vector
	{
		AddObjName(newName);
	}
	else
	{
		RemoveName(oldName);
		AddObjName(newName);
	}
}

void ModuleSceneIntroIntro::TestRayHitObj(LineSegment line)
{
	//Ray ray = line.ToRay();
	bool hasHitAnything = false;
	bool selectAddMode = false;
	bool cancelSelectionMode = false;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		selectAddMode = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
	{
		cancelSelectionMode = true;
	}


	std::vector<GameObject*> allObjs;

	//we use the multimap in case there are more than 1 object where the ray intersects them in the same place
	std::multimap<float, GameObject*> hitObjAABBs;

	root->GetObjAndAllChilds(allObjs); //first we get all scene objects
	allObjs.erase(allObjs.begin());//discards scene root

	//then we test if any of the game object AABB has collision with the ray
	//and save the game Object in a map with the distance hit value as a key
	for (int i = 0; i < allObjs.size(); i++)
	{
		float nearHit = -1;
		float farHit = -1;

		bool hit = line.Intersects(allObjs[i]->GetWorldAABB(), nearHit, farHit);

		if (hit)
		{
			hitObjAABBs.insert(std::pair<float, GameObject*>(nearHit, allObjs[i]));
		}

	}

	//for each hit AABB in order from nearer to furthest we test each triangle of the mesh to see if it hits
	//if it does we change the Game object to be selected, otherwise we pass onto the next object
	while (!hitObjAABBs.empty())
	{
		GameObject* currObj = hitObjAABBs.begin()->second;


		std::vector<ModuleMesh*> meshes = currObj->GetComponents<ModuleMesh>();
		//if has any mesh
		if (!meshes.empty())
		{
			bool hit = false;

			LineSegment localRay = line;
			float4x4 transf = currObj->GetComponent<ModuleTransform>()->GetGlobalTransform().Inverted();
			localRay.Transform(transf);//we convert the ray into local space

			//test collision for every mesh in the object
			for (int i = 0; i < meshes.size(); i++)
			{
				ResourceMesh* currMesh = meshes[i]->GetMesh();
				if (currMesh != nullptr)
				{

					//Test mesh triangles here
					std::pair<float, float3>lastBestHit = std::pair<float, float3>(floatMax, float3(floatMax, floatMax, floatMax));

					for (int v = 0; v < currMesh->indices.size(); v += 3)
					{
						Triangle tri;
						//construct triangle
						//index 1
						float3 vertex1;
						vertex1.x = currMesh->vertices[(currMesh->indices[v] * 3)];
						vertex1.y = currMesh->vertices[(currMesh->indices[v] * 3) + 1];
						vertex1.z = currMesh->vertices[(currMesh->indices[v] * 3) + 2];
						tri.a = vertex1;

						//index 2
						float3 vertex2;
						vertex2.x = currMesh->vertices[(currMesh->indices[v + 1] * 3)];
						vertex2.y = currMesh->vertices[(currMesh->indices[v + 1] * 3) + 1];
						vertex2.z = currMesh->vertices[(currMesh->indices[v + 1] * 3) + 2];
						tri.b = vertex2;

						//index 3
						float3 vertex3;
						vertex3.x = currMesh->vertices[(currMesh->indices[v + 2] * 3)];
						vertex3.y = currMesh->vertices[(currMesh->indices[v + 2] * 3) + 1];
						vertex3.z = currMesh->vertices[(currMesh->indices[v + 2] * 3) + 2];
						tri.c = vertex3;

						//test triangle
						float nearHit = -1;
						float3 intersectionP;

						hit = (hit | localRay.Intersects(tri, &nearHit, &intersectionP));

						if (nearHit < lastBestHit.first)
						{
							
							lastBestHit = std::pair<float, float3>(nearHit, intersectionP);
						}


					}





					currMesh = nullptr;
				}
			}

			if (hit)
			{
				if (cancelSelectionMode)
				{
					RemoveGameObjFromSelected(currObj);
				}
				else
				{
					SetSelectedGameObject(currObj, selectAddMode);
				}

				hasHitAnything = true;
				break;
			}

			
		}
		else 
		{
			//select game object
			if (cancelSelectionMode)
			{
				RemoveGameObjFromSelected(currObj);
			}
			else
			{
				SetSelectedGameObject(currObj, selectAddMode);
			}

			hasHitAnything = true;
			break;
		}

		meshes.clear();
		currObj = nullptr;
		hitObjAABBs.erase(hitObjAABBs.begin());
	}



	if (hasHitAnything == false && !selectAddMode)//when on add mode we do not want to make all the objects un-select on a missclick 
	{

		for (int i = 0; i < selectedGameObjs.size(); i++)
		{
			selectedGameObjs[i]->focused = false;
			selectedGameObjs[i]->selected = false;
		}
		selectedGameObjs.clear();
	}

	allObjs.clear();
	hitObjAABBs.clear();

}

int ModuleSceneIntroIntro::DoesNameExist(std::string name)
{
	int ret = -1;
	for (int i = 0; i < objNames.size(); i++)
	{
		if (objNames[i] == name)
		{
			ret = i;
			break;
		}
	}
	return ret;
}

//makes a unique name for an object adding '.' + 3 digits when necessary
void ModuleSceneIntroIntro::MakeNameUnique(std::string& name)
{
	std::string currSuffix;
	std::string baseName;
	std::string auxName = name;

	int dotPos = auxName.find_last_of(".");

	if (dotPos >= 0 && dotPos < auxName.size())//if dot exists
	{
		currSuffix = auxName.substr(dotPos + 1);

		int letterPos = currSuffix.find_first_not_of("0123456789");

		if (letterPos < currSuffix.size())//if there are letters after the dot default first .num
		{
			auxName += ".001";
		}
		else//if after the dot there are only numbers
		{
			baseName = auxName.substr(0, dotPos);

			//1.find the current number
			int suffixNum = atoi(currSuffix.c_str());

			//2.take the number and add it one
			suffixNum++;

			if (suffixNum < 1000)
			{

				char newNums[16];
				sprintf_s(newNums, 16, ".%03i", suffixNum);

				auxName = baseName + newNums;
			}
			else
			{
				auxName += ".001";//default first .num
			}



			//3.find if it exists
			//repeat from 2 until no num exists

		}
	}
	else //default first .num
	{
		auxName += ".001";
	}


	int where = DoesNameExist(auxName);
	if (where == -1) //if name isn't found in the lsit assign it
	{
		name = auxName;
	}
	else //else repeat the process
	{
		MakeNameUnique(auxName);
		name = auxName;
	}

	currSuffix.clear();
	baseName.clear();
	auxName.clear();

}
