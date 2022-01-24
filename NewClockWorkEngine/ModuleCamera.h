#ifndef __ModuleCamera__
#define __ModuleCamera__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Color.h"

class ModuleCamera:public Component
{
public:

	ModuleCamera(GameObject* owner,unsigned int ID,Color backgroundCol=Color(0.05f, 0.05f, 0.1f));
	ModuleCamera(GameObject* owner, float nPlaneDist, float fPlaneDist, float foV,float aspectRatio=0.0f, Color backgroundCol = Color(0.05f, 0.05f, 0.1f));
	~ModuleCamera();

	bool Update(float dt);

	void OnEditor();

	void CalcCamPosFromTransformMat(float4x4 &gTransform);
	void CalcCamPosFromDirections(float3 pos, float3 front, float3 up);
	float4x4 GetProjMat()const;
	float4x4 GetViewMat()const;

	void SetNewAspectRatio(int width, int height);
	void SetNewAspectRatio(float aspectRatio);
	void SetNewFoV(float foV);
	void SetNearPlane(float dist);
	void SetFarPlane(float dist);

	float GetNearPlaneDist()const;
	float GetFarPlaneDist()const;
	float GetFoV()const;

	float GetAspectRatio()const;
	float GetInvAspectRatio()const;
	const Frustum& GetFrustum() const;

	void GetFrustumPoints(std::vector<float3>& emptyVector);

	bool GetIsCulling()const;
	void SetAsCullingCam(bool newState);

	Color GetBackgroundCol()const;
	void SetBackgroundCol(Color c);

private:
	void UpdateProjectionMat();

private:
	bool isCulling;
	float nearPlaneDist;
	float farPlaneDist;
	float FoVx;//This is the horizontal FoV which the user changes
	float FoVy;//Vertical FoV, the user cannot change this
	float invAspectRatio;

	Frustum frustum;
	Color backgroundCol;
	float4x4 projectionMatrix;//this matrix is already transposed for use in opengl
};

#endif // !__ModuleCamera__
