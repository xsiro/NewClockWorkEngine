#pragma once
#include "ModuleComponent.h"
#include "GameObject.h"

ModuleComponent::ModuleComponent(ComponentType type) : type(type)
{

}

ModuleComponent::ModuleComponent(ComponentType type, GameObject* owner) : owner(owner), type(type)
{

}

ModuleComponent::~ModuleComponent()
{

}


void ModuleComponent::Update() {

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

ComponentType ModuleComponent::ReturnType() {

	return type;

}

GameObject* ModuleComponent::ReturnGameObject() {

	return owner;

}
