#pragma once
class GameObject;

	enum class ComponentType
	{
		TRANSFORM,
		MESH,
		MATERIAL,
		CAMERA,
		AUDIO_SOURCE,
		AUDIO_LISTENER,
		CONTROL,
		REVERB_ZONE
	};

class Component
{
public:

	Component(ComponentType type,GameObject* owner,unsigned int ID=0);
	virtual ~Component();
	virtual bool Update(float dt);
	virtual bool GameUpdate(float dt);
	virtual bool GameInit();

	virtual void OnEditor();
	
	ComponentType GetType()const;

	void SetActive(bool active);
	bool IsActive()const;

	virtual void SetNewResource(unsigned int resourceID);
	virtual unsigned int GetResourceID();

public:
	GameObject* owner;
	bool toDelete;
	unsigned int ID;

private:
	ComponentType type;
protected:
	bool active;
};

