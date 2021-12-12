#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"

class ModuleComponent;
struct ResourceMaterial;
class ConfigNode;

class ModuleMaterial : public ModuleComponent
{
public:

	ModuleMaterial(GameObject* owner);
	ModuleMaterial(GameObject* owner, const char* path, ResourceMaterial* mesh);;
	~ModuleMaterial();

	void Update() override;
	void CleanUp() override;
	void DrawInspector() override;
	void OnSave(ConfigNode* node);

	const char* GetPath()const;

	ResourceMaterial* GetTexture() const;

	bool IsEnabled() const;

	static inline ComponentType GetType() { return ComponentType::Material; };
	
	void SwitchEnabledTexture();


private:

	ResourceMaterial* material = nullptr;
	const char* path = "";
	bool drawTexture = true;

};