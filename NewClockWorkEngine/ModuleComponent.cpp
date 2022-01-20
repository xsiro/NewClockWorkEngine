#pragma once
#include "ModuleComponent.h"
#include "GameObject.h"

ModuleComponent::ModuleComponent(ComponentType type) : type(type)
{

}

ModuleComponent::ModuleComponent(ComponentType type, GameObject* owner, unsigned int ID) : owner(owner), type(type)
{

}

ModuleComponent::~ModuleComponent()
{

}


void ModuleComponent::Update() {

}

bool ModuleComponent::GameUpdate(float dt)
{
	return true;
}

bool ModuleComponent::GameInit()
{
	return true;
}

void ModuleComponent::Enable() 
{
	active = true;
}

void ModuleComponent::Disable()
{

	active = false;
}

bool ModuleComponent::IsActive()
{
	return active;
}

ModuleComponent::ComponentType ModuleComponent::ReturnType() {

	return type;

}

GameObject* ModuleComponent::ReturnGameObject() {

	return owner;

}
