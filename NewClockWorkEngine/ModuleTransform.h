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

	float4x4 GetTransform() const;
	float4x4 GetGlobalTransform()const;
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
	float4x4 globalTransform;

	float3 position;
	float3 scale;
	Quat rotation;

	float3 eulerRotation;

	float3 eulerRotationUi;
	float3 positionUI;

};