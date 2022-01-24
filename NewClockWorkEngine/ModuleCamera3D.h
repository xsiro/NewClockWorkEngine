#ifndef __CAMERA_3D__
#define __CAMERA_3D__


#include "Module.h"
#include "Globals.h"
//#include "glmath.h"

//class vec3;
//class mat4x4;




enum class CamObjective
{
	REFERENCE,
	CAMERA
};


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt) override;
	bool CleanUp();

	//void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	//void LookAt(const vec3 &Spot);
	void Move(const float3 &Movement);
	void MoveTo(const float3& Destination,CamObjective toMove);
	void CamZoom(int addZoomAmount);
	//float* GetRawViewMatrix();
	//mat4x4 GetViewMatrix();

	void CreateRayFromScreenPos(float normalizedX, float normalizedY);

private:

	//void CalculateViewMatrix();
	float3 Rotate(const float3& u, float angle, const float3& v);
public:
	
	float3 X, Y, Z, Position, Reference;
	//float foV;
	//float nearPlaneDist;
	//float farPlaneDist;
	//bool debugcamera;

	float camSpeed;
	float camSpeedMult;

	//NEW
	ModuleCamera* editorCam;
	float2 lastKnowMousePos;
	bool viewportClickRecieved;
	bool isGizmoInteracting; //TODO too many bools, change the way the inputs to the camera are sent and recieved

private:
	float zoomLevel;
	//mat4x4 ViewMatrix, ViewMatrixInverse;

};
#endif // !__CAMERA_3D__