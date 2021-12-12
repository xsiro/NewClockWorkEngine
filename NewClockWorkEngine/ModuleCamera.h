
#include "MathGeoLib/src/Geometry/Plane.h"
#include "MathGeoLib/src/Geometry/Frustum.h"
#include "MathGeoLib/src/Math/float4x4.h"


class ModuleComponent;

class ModuleCamera : public ModuleComponent
{
public:
	ModuleCamera(GameObject* owner);

	void Update()override;
	void CleanUp() override;
	void DrawInspector() override;
	//void OnPlay() override {};
	//void OnStop() override {};

	/*void Serialize(JsonNode* node)override;
	void Load(JsonNode* node)override;*/

	static inline ModuleComponent::ComponentType GetType() { return ModuleComponent::ComponentType::Camera; };

	//near/far planes

	void Setposition(float3 pos);

	void SetNearPlane(float distance);
	void SetFarPlane(float distance);

	void SetVerticalFov(float verticalFov);
	void SetHorizontalFov(float horizontalFov);
	void SetAspectRatio(float ratio);

	float ComputeAspectRatio(float verticalFov, float horizontalFov);

	void OnUpdateTransform(float4x4 globalTransform);

	//get view/projection
	float* GetViewMatrix();

	void UpdatePlanes();

	Frustum GetFrustum()const;
	float3 GetPos()const;
	float GetNearPlaneDistance()const;
	float GetFarPlaneDistance()const;
	float GetVerticalFov()const;
	float GetHorizontalFov()const;


public:

	Frustum frustum;
	bool isCurrentCamera = false;
	bool isCullingCamera = false;
	bool cull = true;
	Plane planes[6];
	vec* corners;
};