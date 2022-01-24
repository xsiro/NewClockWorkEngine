#pragma once
#include "Component.h"
#include "Assimp/include/matrix4x4.h" 
#include "MathGeoLib/include/MathGeoLib.h"

class ModuleTransform : public Component
{
public:

	ModuleTransform(GameObject* owner, float4x4 lTransform,unsigned int ID, bool islocalTrans = true);
	~ModuleTransform();

	float4x4 GetGlobalTransform()const;
	float4x4 GetLocalTransform()const;
	void SetGlobalTransform(float4x4 newGTransform);
	void SetLocalTransform(float4x4 newLTransform);

	void OnEditor();

	float3 GetLocalPosition();
	float3 GetGlobalPosition();
	float3 GetLocalScale();
	float3 GetGlobalScale();


	void SetLocalPosition(float3 newPos);
	void SetLocalScale(float3 newScale);
	void SetLocalRot(Quat newRot);
	void SetGlobalPosition(float3 newPos);
	void SetGlobalScale(float3 newScale);
	void SetGlobalRot(Quat newRot);


	void UpdateGlobalMat();
	void UpdateLocalMat();

public:
	bool localMode;
private:

	float4x4 lTransformMat;
	float4x4 gTransformMat;
};

