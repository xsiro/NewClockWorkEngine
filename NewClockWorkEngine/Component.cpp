#include "Component.h"
#include "Application.h"
#include "ModuleRenderer3D.h"



Component::Component(ComponentType type, GameObject* owner, unsigned int ID) :type(type)
{
	this->owner = owner;
	active = true;
	toDelete = false;
	if (ID == 0)
	{
		if (App != nullptr)
		{
			this->ID = App->renderer3D->seed.Int();
		}
		else
		{
			this->ID = 1;
		}
	}
	else
	{
		this->ID = ID;
	}
}

Component::~Component()
{
}

bool Component::Update(float dt)
{
	if (ID == -1) {
		this->ID = App->renderer3D->seed.Int();
	}
	return true;
}

bool Component::GameUpdate(float dt)
{
	return true;
}

bool Component::GameInit()
{
	return true;
}

void Component::OnEditor()
{
}

ComponentType Component::GetType() const
{
	return type;
}

void Component::SetActive(bool active)
{
	if (type != ComponentType::TRANSFORM)
		this->active = active;
}

bool Component::IsActive() const
{
	return active;
}

void Component::SetNewResource(unsigned int resourceID)
{
}

unsigned int Component::GetResourceID()
{
	return 0;
}
