#pragma once
#include "GameObject.h"
#include "ModuleComponent.h"
#include "ModuleTransform.h"
#include "ModuleMesh.h"
#include "ModuleMaterial.h"
#include "imgui.h"

#include <vector>

GameObject::GameObject(char* name) : name(name)
{
	AddComponent(new ModuleTransform(this));
}

GameObject::GameObject(GameObject* parent = nullptr, char* name = "Object") : parent(parent), name(name)
{
	AddComponent(new ModuleTransform(this));
}

GameObject::~GameObject()
{
	
}

void GameObject::Update() 
{
	std::vector<ModuleComponent*>::iterator item = components.begin();
	for (; item != components.end(); ++item)
	{
		(*item)->Update();
	}

	std::vector<GameObject*>::iterator child = children.begin();
	for (; child != children.end(); ++child)
	{
		(*child)->Update();
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

	std::vector<GameObject*>::iterator child = children.begin();
	for (; child != children.end(); ++child)
	{
		(*child)->CleanUp();
		delete (*child);
	}

	children.clear();
}

ModuleComponent* GameObject::GetComponent(ComponentType component) 
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


void GameObject::DeleteComponent(ComponentType type)
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
	ComponentType type = component->ReturnType();

	switch (type)
	{
	case ComponentType::Transform:

		components.push_back(component);
		transform = (ModuleTransform*)component;
		break;
		case ComponentType::Mesh:

		if (!HasComponentType(ComponentType::Mesh))
		{
			components.push_back(component);
		}
		else
			LOG("(ERROR) Error adding Mesh: Object already has Mesh");

		break;

	case ComponentType::Material:

		if (!HasComponentType(ComponentType::Material))
		{
			components.push_back(component);
			material = (ModuleMaterial*)component;
		}
		else
		{
			DeleteComponent(ComponentType::Material);
			components.push_back(component);
			material = (ModuleMaterial*)component;
		}

		break;
	}

	return component;
}

bool GameObject::HasComponentType(ComponentType type)
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