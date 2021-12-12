#pragma once
#pragma once
#include "Globals.h"
#include <vector>
#include "ModuleComponent.h"
#include "MathGeoLib/src/MathGeoLib.h"

class ModuleComponent;
class ConfigNode;

class ModuleTransform : public ModuleComponent
{
public:

	ModuleTransform(GameObject* owner);
	~ModuleTransform();

	void Update() override;
	void CleanUp() override;
	void DrawInspector()override;

	static inline ModuleComponent::ComponentType GetType() { return ModuleComponent::ComponentType::Transform; };

	float4x4 GetTransform() const;
	float3 GetPosition()const;
	float3 GetScale()const;

	void SetPosition(float3 position);
	void SetScale(float3 scale);
	void SetLocalTransform(float3 position, float3 scale, Quat rotation);
	void SetTransform(float3 position, float3 scale, Quat rotation);
	void RecalculateMatrix();
	void SetEulerRotation(float3 euler_angles);
	void UpdateTRS();
	void UpdatedTransform(float4x4 parentGlobalTransform);
	void RecalculateEuler();
	void UpdateLocalTransform();
	void OnSave(ConfigNode* node);
	float4x4 GetGlobalTransform()const;

private:

	float4x4 transform;
	float4x4 globalTransform;

	float3 position;
	float3 scale;
	Quat rotation;

	float3 eulerRotation;
	float3 eulerRotationUi;
	float3 positionUI;

public:

	bool updateTransform = false;
};