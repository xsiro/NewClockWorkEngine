#pragma once
#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "ModuleComponent.h"

class ModuleComponent;
struct Material;

class ModuleMaterial : public ModuleComponent
{
public:

	ModuleMaterial(GameObject* owner);
	ModuleMaterial(GameObject* owner, char* path, Material* mesh);;
	~ModuleMaterial();
	void Update() override;
	void CleanUp() override;
	void DrawInspector() override;

	char* GetPath()const;
	Material* GetTexture() const;
	bool IsEnabled();
	void SwitchEnabledTexture();


private:

	Material* material = nullptr;
	char* path = "";
	bool drawTexture = true;

};