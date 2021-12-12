#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Geometry/LineSegment.h"

class ModuleCamera;
class GameObject;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void SetCurrentCamera(ModuleCamera* newCamera);
	void SetCullingCamera(ModuleCamera* newCamera);
	void SetPosition(float3 newPosition);
	void RaycastSelect();
	void CheckIntersetions(LineSegment* selectRay);
	void RotateCameraStatic();

private:

	void CalculateViewMatrix();

public:

	GameObject* mainCameraObject = nullptr;		//Object holding mainCamera
	ModuleCamera* cullingCamera = nullptr;		//Camera to be used outside Play Mode
	ModuleCamera* currentCamera = nullptr;		//Camera currently being used, modify this camera to move

	float cameraMoveSpeed = 1;
	float cameraRotateSpeed = 1;
	
	vec3 X, Y, Z, Position, Reference;
	Color background;
	bool free_camera = false;
	bool rotate_camera = false;
private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};