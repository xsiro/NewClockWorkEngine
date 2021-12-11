#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"

class ModuleComponent;
struct ResourceMaterial;

class ModuleMaterial : public ModuleComponent
{
public:

	ModuleMaterial(GameObject* owner);
	ModuleMaterial(GameObject* owner, const char* path, ResourceMaterial* mesh);;
	~ModuleMaterial();
	void Update() override;
	void CleanUp() override;
	void DrawInspector() override;

	const char* GetPath()const;
	ResourceMaterial* GetTexture() const;
	bool IsEnabled() const;
	void SwitchEnabledTexture();

	static inline ComponentType GetType() { return ComponentType::Material; };


private:

	ResourceMaterial* material = nullptr;
	const char* path = "";
	bool drawTexture = true;

};