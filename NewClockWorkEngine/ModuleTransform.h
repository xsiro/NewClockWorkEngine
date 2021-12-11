#pragma once
#pragma once
#include "Globals.h"
#include <vector>
#include "ModuleComponent.h"
#include "MathGeoLib/src/MathGeoLib.h"

class ModuleComponent;

class ModuleTransform : public ModuleComponent
{
public:
	ModuleTransform(GameObject* owner);
	~ModuleTransform();

	void Update() override;
	void CleanUp() override;
	void DrawInspector()override;

	static inline ComponentType GetType() { return ComponentType::Transform; };

	float4x4 GetTransform() const;
	float3 GetPosition()const;
	float3 GetScale()const;

	void SetPosition(float3 position);
	void SetScale(float3 scale);
	void RecalculateMatrix();
	void SetEulerRotation(float3 euler_angles);
	void UpdateTRS();

	void RecalculateEuler();

private:

	float4x4 transform;

	float3 position;
	float3 scale;
	Quat rotation;

	float3 eulerRotation;

	float3 eulerRotationUi;
	float3 positionUI;

};