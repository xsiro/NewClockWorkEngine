#include "GameObject.h"
#include "Component.h"
#include "ModuleTransform.h"
#include "ModuleMesh.h"
#include "ModuleMaterial.h"
#include "ModuleControl.h"
#include "ModuleReverbZone.h"
#include "ModuleCamera.h"
#include "AudioSource.h"
#include "AudioListener.h"
#include "Application.h"
#include "imgui/imgui.h"

#include "MathGeoLib/include/MathGeoLib.h"


int GameObject::numberOfObjects = 0;


GameObject::GameObject(GameObject* parent, std::string name, float4x4 transform, bool showAABB, bool isLocalTrans) :name(name), transform(nullptr), focused(false), selected(false), displayBoundingBox(showAABB)
{
	App->scene->AddObjName(this->name);
	this->parent = parent;
	isActive = true;
	bbHasToUpdate = true;
	if (parent)
	{
		parent->children.push_back(this);
	}
	this->transform = new ModuleTransform(this, transform, 0, isLocalTrans);
	components.push_back(this->transform);

	numberOfObjects++;

	globalAABB.SetNegativeInfinity();
	globalOBB.SetNegativeInfinity();

	this->ID = App->renderer3D->seed.Int(); //begone T H O T
}

void GameObject::Awake()
{
}

void GameObject::GameInit()
{
	for (int i = 0; i < components.size(); i++)
	{

		components[i]->GameInit();

	}
	for (int i = 0; i < children.size(); i++)
	{

		children[i]->GameInit();

	}
}


void GameObject::Update(float dt)
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->toDelete)
		{
			delete components[i];
			components[i] = nullptr;
			components.erase(components.begin() + i);
			i--;
		}
	}

	if (isActive)
	{

		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->IsActive())
			{
				components[i]->Update(dt);
			}
		}



		for (int i = 0; i < children.size(); i++)
		{
			if (children[i]->isActive)
			{
				children[i]->Update(dt);
			}
		}

		if (bbHasToUpdate)
		{
			UpdateBoundingBox();
		}
		DrawGameObject();
	}
}

void GameObject::GameUpdate(float gameDt)
{
	if (isActive)
	{

		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->IsActive())
			{
				components[i]->GameUpdate(gameDt);
			}
		}



		for (int i = 0; i < children.size(); i++)
		{
			if (children[i]->isActive)
			{
				children[i]->GameUpdate(gameDt);
			}
		}
	}
}

GameObject::~GameObject()
{
	App->scene->RemoveName(name);

	numberOfObjects--;

	for (int i = 0; i < components.size(); i++)
	{
		if (components[i] != transform)
		{
			delete components[i];
			components[i] = nullptr;
		}
	}
	components.clear();

	if (transform != nullptr)//This wont be needed as transform is deleted from the component vector
		delete transform;

	transform = nullptr;

	while (!children.empty())
	{
		GameObject* child = children.back();
		children.pop_back();
		delete child;
	}
	children.clear();

	RemoveMyselfFromParent();

	if (App)
		App->scene->RemoveGameObjFromSelected(this);

}

void GameObject::RemoveChildren(GameObject* toRemove)
{
	std::vector<GameObject*>::iterator iterator = children.begin();

	for (iterator; iterator != children.end(); iterator++)
	{
		if (*iterator == toRemove)
		{

			children.erase(iterator);

			break;
		}

	}

}

void GameObject::ChangeParent(GameObject* newParent)
{
	RemoveMyselfFromParent();
	//newParent->children

	if (newParent == nullptr)
	{
		parent = App->scene->root;
	}
	else
	{
		parent = newParent;
	}

	parent->children.push_back(this);

	ModuleTransform* transform = GetComponent<ModuleTransform>();
	transform->UpdateLocalMat();
	UpdateChildTransforms();

}

void GameObject::RemoveMyselfFromParent()
{
	if (parent)
	{
		parent->RemoveChildren(this);
	}
}

Component* GameObject::CreateComponent(ComponentType type, unsigned int compID)
{
	Component* ret = nullptr;
	//TODO add diferent components here
	switch (type)
	{
	case ComponentType::MESH:
		ret = new ModuleMesh(this, compID);
		break;
	case ComponentType::MATERIAL:
		//only one instance of material for a certain gameObj
		if (GetComponent<ModuleMaterial>() == nullptr)
			ret = new ModuleMaterial(this, compID);
		break;
	case ComponentType::CAMERA:
		//only one instance of camera for a certain gameObj
		if (GetComponent<ModuleCamera>() == nullptr)
			ret = new ModuleCamera(this, compID);
		break;
	case ComponentType::AUDIO_LISTENER:
		//only one instance of a listener for a certain gameObj
		if (GetComponent<AudioListener>() == nullptr)
			ret = new AudioListener(this, compID);
		break;
	case ComponentType::AUDIO_SOURCE:
		//only one instance of a audio source for a certain gameObj
		if (GetComponent<AudioSource>() == nullptr)
			ret = new AudioSource(this, compID);
		break;
	case ComponentType::CONTROL:
		if (GetComponent<ModuleControl>() == nullptr)
			ret = new ModuleControl(this, compID);
		break;
	case ComponentType::REVERB_ZONE:
		if (GetComponent<ModuleReverbZone>() == nullptr)
			ret = new ModuleReverbZone(this, compID);
		break;

	default:
		break;
	}


	if (ret)
	{
		components.push_back(ret);
	}

	return ret;
}

std::string GameObject::GetName()
{
	return this->name;
}

//recursive function that returns false if some game object from which this inherits is not active 
bool GameObject::IsParentActive()
{
	if (!isActive)
		return false;

	if (parent)
	{
		return parent->IsParentActive();
	}

	return isActive;
}

void GameObject::GetChildWithID(unsigned int ID, GameObject*& childOut)//Note that child out must be nullptr at the start
{
	if (this->ID == ID)
	{
		childOut = this;
	}
	else
	{
		for (int i = 0; i < children.size(); i++)
		{
			if (childOut == nullptr)
			{
				children[i]->GetChildWithID(ID, childOut);
			}
		}
	}
}

void GameObject::UpdateChildTransforms()
{
	GetComponent<ModuleTransform>()->UpdateGlobalMat();
	bbHasToUpdate = true;
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->UpdateChildTransforms();
	}
}

void GameObject::UpdateBoundingBox()
{
	ModuleMesh* mesh = GetComponent <ModuleMesh>(); //TODO for now we will only make it that the first mesh draws the bounding box, add support for multiple boundingboxes (if more than 1 mesh)

	if (mesh != nullptr)
	{
		globalOBB = mesh->GetAABB();
		globalOBB.Transform(GetComponent< ModuleTransform>()->GetGlobalTransform());

		globalAABB.SetNegativeInfinity();
		globalAABB.Enclose(globalOBB);
	}
	else
	{
		globalAABB.SetNegativeInfinity();
		globalAABB.SetFromCenterAndSize(transform->GetGlobalPosition(), float3(1, 1, 1));
		globalOBB = globalAABB;
	}



	bbHasToUpdate = false;
}

void GameObject::GetObjAndAllChilds(std::vector<GameObject*>& childs)
{
	childs.push_back(this);

	for (int i = 0; i < children.size(); i++)
	{
		children[i]->GetObjAndAllChilds(childs);
	}


}

AABB GameObject::GetWorldAABB() const
{
	return globalAABB;
}

void GameObject::DrawOnEditorAllComponents()
{
	if (ImGui::BeginChild("Object name window", ImVec2(0.0f, 30.0f)))
	{
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Checkbox(" ", &isActive);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Active");
			ImGui::EndTooltip();
		}


		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10);
		char* auxName = (char*)name.c_str();
		std::string oldName = name;
		if (ImGui::InputText("Object Name", auxName, 100))
		{
			name = auxName;
			name.shrink_to_fit();
			if (name.empty())
			{
				name = "Untitled";
			}
			App->scene->ChangeObjName(oldName, name);
		}
	}
	ImGui::EndChild();
	//ImGui::Text("My name is %s", name.c_str());
	ImGui::Separator();
	ImGui::Spacing();


	for (int i = 0; i < this->components.size(); i++)
	{
		components[i]->OnEditor();

	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();

	int buttonWidth = 150;
	float wWidth = ImGui::GetWindowWidth();
	ImVec2 cursPos = ImGui::GetCursorPos();
	cursPos.x = cursPos.x + (wWidth * 0.5f) - (buttonWidth * 0.5f); //60 is half button width
	ImGui::SetCursorPos(cursPos);

	if (ImGui::Button("Add Component##objComponent", ImVec2(buttonWidth, 20)))
	{
		ImGui::OpenPopup("AddComponent##Popup");

	}
	ImGui::SameLine();
	if (ImGui::BeginPopup("AddComponent##Popup"))
	{
		ImGui::Text("Select New Component to Add");
		ImGui::Separator();

		//TODO this can be made pretty in the future
		if (GetComponent<ModuleMesh>() == nullptr)//support multiple meshes in the future?
		{
			if (ImGui::Selectable("Mesh Component##addComponent"))
				CreateComponent(ComponentType::MESH);
		}
		if (GetComponent<ModuleMaterial>() == nullptr)
		{
			if (ImGui::Selectable("Material Component##addComponent"))
				CreateComponent(ComponentType::MATERIAL);
		}
		if (GetComponent<ModuleCamera>() == nullptr)
		{
			if (ImGui::Selectable("Camera Component##addComponent"))
				CreateComponent(ComponentType::CAMERA);
		}
		if (GetComponent<AudioListener>() == nullptr)
		{
			if (ImGui::Selectable("Audio Listener Component##addComponent"))
				CreateComponent(ComponentType::AUDIO_LISTENER);
		}
		if (GetComponent<AudioSource>() == nullptr)
		{
			if (ImGui::Selectable("Audio Source Component##addComponent"))
				CreateComponent(ComponentType::AUDIO_SOURCE);
		}
		if (GetComponent<ModuleControl>() == nullptr)
		{
			if (ImGui::Selectable("Control Component##addComponent"))
				CreateComponent(ComponentType::CONTROL);
		}
		if (GetComponent<ModuleReverbZone>() == nullptr)
		{
			if (ImGui::Selectable("ReverbZone Component##addComponent"))
				CreateComponent(ComponentType::REVERB_ZONE);
		}



		ImGui::EndPopup();
	}
}

std::vector<Component*> GameObject::GetAllComponents()
{
	return this->components;
}

void GameObject::DrawGameObject()
{
	std::vector<float3> aabbVec;
	GetPointsFromAABB(globalAABB, aabbVec);

	if (App->renderer3D->IsInsideFrustum(aabbVec))
	{


		std::vector<ModuleMesh*>meshes = GetComponents<ModuleMesh>();

		ModuleMaterial* mat = GetComponent<ModuleMaterial>();
		if (mat != nullptr && !mat->IsActive())
		{
			mat = nullptr;
		}

		for (int i = 0; i < meshes.size(); i++)
		{
			if (meshes[i]->IsActive() && meshes[i]->GetMesh() != nullptr)
			{
				App->renderer3D->AddMeshToDraw(meshes[i], mat, transform->GetGlobalTransform(), selected);
			}

		}


	}

	if (App->scene->root != this && (App->renderer3D->displayAABBs || (displayBoundingBox && (focused || selected))))
	{
		Color c = Color(1.0f, 1.0f, 1.0f, 1.0f);
		if (focused)
			c = Color FOCUSED_COLOR;
		else if (selected)
			c = Color SELECTED_COLOR;

		App->renderer3D->AddBoxToDraw(aabbVec, c);
	}

	ModuleCamera* cam = GetComponent<ModuleCamera>();
	if (cam && cam->IsActive())
	{
		std::vector<float3> vec;
		cam->GetFrustumPoints(vec);
		Color c = Color(1.0f, 1.0f, 1.0f, 1.0f);
		if (cam->GetIsCulling())
			c = Color(0.0f, 0.75f, 0.75f, 1.0f);

		App->renderer3D->AddBoxToDraw(vec, c);
	}
}

//Takes an aabb and fills empty vector with its points
void GameObject::GetPointsFromAABB(AABB& aabb, std::vector<float3>& emptyVector)
{
	float3* frustrumPoints = new float3[8];
	memset(frustrumPoints, NULL, sizeof(float3) * 8);
	aabb.GetCornerPoints(frustrumPoints);

	emptyVector.clear();

	for (int i = 0; i < 8; i++)
	{
		emptyVector.push_back(frustrumPoints[i]);
	}
	delete[]frustrumPoints;
	frustrumPoints = nullptr;
}
