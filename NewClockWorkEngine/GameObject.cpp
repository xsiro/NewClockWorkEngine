#pragma once
#include "GameObject.h"
#include "ModuleComponent.h"
#include "ModuleTransform.h"
#include "ModuleMesh.h"
#include "ModuleMaterial.h"
#include "imgui.h"
#include "Application.h"



GameObject::GameObject(const char* name) : name(name)
{
	AddComponent(new ModuleTransform(this));
}

GameObject::GameObject(GameObject* parent = nullptr, const char* name = "Object") : parent(parent), name(name)
{
	AddComponent(new ModuleTransform(this));
}

GameObject::~GameObject()
{
	
}

void GameObject::Update() 
{
	//UpdateBoundingBoxes();

	if (!components.empty())
	{
		std::vector<ModuleComponent*>::iterator item = components.begin();
		for (; item != components.end(); ++item)
		{
			(*item)->Update();
		}
	}
}



void GameObject::CleanUp()
{

	std::vector<ModuleComponent*>::iterator item = components.begin();
	for (; item != components.end(); ++item)
	{
		(*item)->CleanUp();
		delete (*item);
	}

	components.clear();
	children.clear();

}

//
//ModuleMesh* GameObject::GetComponentMesh()
//{
//	ModuleComponent* mesh = nullptr;
//	for (std::vector<ModuleComponent*>::iterator i = components.begin(); i != components.end(); i++)
//	{
//		if ((*i)->type == ComponentType::Mesh)
//		{
//			return (ModuleMesh*)*i;
//		}
//	}
//	return (ModuleMesh*)mesh;
//}

//
//ModuleTransform* GameObject::GetComponentTransform()
//{
//	ModuleComponent* transform = nullptr;
//	for (std::vector<ModuleComponent*>::iterator i = components.begin(); i != components.end(); i++)
//	{
//		if ((*i)->type == ComponentType::Transform)
//		{
//			return (ModuleTransform*)*i;
//		}
//	}
//	return (ModuleTransform*)transform;
//}


void GameObject::DeleteComponent(ModuleComponent::ComponentType type)
{
	std::vector<ModuleComponent*>::iterator item = components.begin();
	for (; item != components.end(); ++item)
	{
		if ((*item)->ReturnType() == type)
		{
			components.erase(item);
			return;

		}
	}
}

std::vector<ModuleComponent*> GameObject::GetComponents()const
{
	return components;
}

ModuleComponent* GameObject::AddComponent(ModuleComponent* component)
{
	ModuleComponent::ComponentType type = component->ReturnType();

	switch (type)
	{
	case ModuleComponent::ComponentType::Transform:

		components.push_back(component);
		transform = (ModuleTransform*)component;
		break;
		case ModuleComponent::ComponentType::Mesh:

		if (!HasComponentType(ModuleComponent::ComponentType::Mesh))
		{
			components.push_back(component);
			//UpdateBoundingBoxes();
		}
		else
			LOG("(ERROR) Error adding Mesh: Object already has Mesh");

		break;

	case ModuleComponent::ComponentType::Material:

		if (!HasComponentType(ModuleComponent::ComponentType::Material))
		{
			components.push_back(component);
			//material = (ModuleMaterial*)component;
		}
		else
		{
			DeleteComponent(ModuleComponent::ComponentType::Material);
			components.push_back(component);
			//material = (ModuleMaterial*)component;
		}

		break;
	}

	return component;
}

bool GameObject::HasComponentType(ModuleComponent::ComponentType type)
{
	bool ret = false;
	std::vector<ModuleComponent*>::iterator item = components.begin();
	for (; item != components.end(); ++item)
	{
		if ((*item)->ReturnType() == type)
		{
			ret = true;
			return ret;
		}
	}

	return ret;
}

const char* GameObject::GetName()
{
	return name.c_str();
}

void GameObject::SetName(const char* name)
{
	this->name = name;
}

void GameObject::Enable()
{
	active = true;
}

void GameObject::Disable()
{
	active = false;
}

bool GameObject::IsActive()
{
	return active;
}

//void GameObject::UpdateBoundingBoxes()
//{
//	if (HasComponentType(ComponentType::Mesh))
//	{
//		obb = GetComponent<ModuleMesh>()->GetMesh()->aabb;
//		obb.Transform(transform->GetGlobalTransform());
//
//		aabb.SetNegativeInfinity();
//		aabb.Enclose(obb);
//	}
//}

//
//void GameObject::DrawBB(bool drawBB)
//{
//	if (drawBB)
//	{
//		App->renderer3D->CreateAABB(aabb, Green);
//		App->renderer3D->CreateOBB(obb, Pink);
//	}
//
//}

void GameObject::UpdatedTransform()
{

	transform->UpdatedTransform(parent->transform->GetGlobalTransform());
	
	
	//call children's on updateTransforms

	std::vector<GameObject*>::iterator child = children.begin();
	for (; child != children.end(); ++child)
	{
		(*child)->UpdatedTransform();
	}
}