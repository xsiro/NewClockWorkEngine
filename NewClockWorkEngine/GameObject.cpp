#pragma once
#include "GameObject.h"
#include "ModuleComponent.h"
#include "ModuleTransform.h"
#include "ModuleMesh.h"
#include "ModuleMaterial.h"
#include "ModuleCamera.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include "imgui/include/imgui.h"
#include "Application.h"
#include "ResourceMesh.h"

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
	UpdateBoundingBoxes();
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

ModuleComponent* GameObject::GetComponent(ModuleComponent::ComponentType component) 
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->ReturnType() == component)
		{
			return components[i];
		}
	}
	return nullptr;
}

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

		if (!HasComponentType(ModuleComponent::ComponentType::Transform))
		{
			components.push_back(component);
			transform = (ModuleTransform*)component;
		}
		else
		{
			component = GetComponent<ModuleTransform>();
		}
		break;

	case ModuleComponent::ComponentType::Mesh:

		if (!HasComponentType(ModuleComponent::ComponentType::Mesh))
		{
			components.push_back(component);
			//UpdateBoundingBoxes();
		}
		else
			component = GetComponent<ModuleMesh>();

		break;

	case ModuleComponent::ComponentType::Material:

		if (!HasComponentType(ModuleComponent::ComponentType::Material))
		{
			components.push_back(component);
		}
		else
		{
			DeleteComponent(ModuleComponent::ComponentType::Material);
			components.push_back(component);
		}

		break;

	case ModuleComponent::ComponentType::Camera:

		if (!HasComponentType(ModuleComponent::ComponentType::Camera))
		{
			components.push_back(component);
		}
		else
		{
			LOG("(ERROR) Error adding Camera: Object already has camera");
			component = GetComponent<ModuleCamera>();
		}
		break;

	case ModuleComponent::ComponentType::Audio_listener:

		if (!HasComponentType(ModuleComponent::ComponentType::Audio_listener))
		{
			components.push_back(component);
		}
		else
		{
			DeleteComponent(ModuleComponent::ComponentType::Audio_listener);
			components.push_back(component);
		}
		break;

	case ModuleComponent::ComponentType::Audio_source:

		if (!HasComponentType(ModuleComponent::ComponentType::Audio_source))
		{
			components.push_back(component);
		}
		else
		{
			DeleteComponent(ModuleComponent::ComponentType::Audio_source);
			components.push_back(component);
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


void GameObject::UpdatedTransform()
{
	transform->UpdatedTransform(parent->transform->GetGlobalTransform());

	std::vector<GameObject*>::iterator child = children.begin();
	for (; child != children.end(); ++child)
	{
		(*child)->UpdatedTransform();
	}
}

ModuleMesh* GameObject::GetComponentMesh()
{
	ModuleComponent* mesh = nullptr;

	for (std::vector<ModuleComponent*>::iterator i = components.begin(); i != components.end(); i++)
	{
		if ((*i)->type == ModuleComponent::ComponentType::Mesh)
		{
			return (ModuleMesh*)*i;
		}
	}
	return (ModuleMesh*)mesh;
}

ModuleTransform* GameObject::GetComponentTransform()
{
	ModuleComponent* transform = nullptr;

	for (std::vector<ModuleComponent*>::iterator i = components.begin(); i != components.end(); i++)
	{
		if ((*i)->type == ModuleComponent::ComponentType::Transform)
		{
			return (ModuleTransform*)*i;
		}
	}
	return (ModuleTransform*)transform;
}

void GameObject::UpdateBoundingBoxes()
{
	if (HasComponentType(ModuleComponent::ComponentType::Mesh))
	{
		ResourceMesh* mesh = GetComponent<ModuleMesh>()->GetMesh();
		if (mesh != nullptr)
		{
			obb = mesh->aabb;
			obb.Transform(transform->GetGlobalTransform());

			aabb.SetNegativeInfinity();
			aabb.Enclose(obb);
		}
	}
}


