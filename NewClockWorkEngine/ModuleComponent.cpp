#pragma once
#include "ModuleComponent.h"

ModuleComponent::ModuleComponent(ComponentType type) : type(type)
{

}

ModuleComponent::ModuleComponent(ComponentType type, GameObject* owner) : owner(owner), type(type)
{

}

ModuleComponent::~ModuleComponent()
{

}

void ModuleComponent::Enable() {

	active = true;

}

void ModuleComponent::Update() {

}

void ModuleComponent::Disable() {

	active = false;
}

ComponentType ModuleComponent::ReturnType() {

	return type;

}

GameObject* ModuleComponent::ReturnGameObject() {

	return owner;

}
