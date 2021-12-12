#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "GameObject.h"
#include "ModuleComponent.h"
#include "ModuleCamera.h"
#include <map>
#include "SDL.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	currentCamera = new ModuleCamera(nullptr);

	cameraMoveSpeed = 7.5f;
	cameraRotateSpeed = 60.f;

	SetCurrentCamera(currentCamera);
	SetCullingCamera(currentCamera);

	currentCamera->Setposition(float3(0, 5, -5));

	LOG("Setting up the camera");
	bool ret = true;

	mainCameraObject = App->scene_intro->CreateGameObject("MainCameraObject", App->scene_intro->rootObject);
	mainCameraObject->AddComponent(currentCamera);

	return ret;
}


// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	float3 newPos(0, 0, 0);
	float speed = cameraMoveSpeed * dt;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = (cameraMoveSpeed * 2.f) * dt;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += currentCamera->frustum.Front() * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= currentCamera->frustum.Front() * speed;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= currentCamera->frustum.WorldRight() * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += currentCamera->frustum.WorldRight() * speed;


	if (!App->gui->IsMouseHovering())
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			RaycastSelect();
	}

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
		RotateCameraStatic();

	/*if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		OrbitObject();*/


	currentCamera->Setposition(currentCamera->frustum.Pos() + newPos);

	return UPDATE_CONTINUE;
	

	//// Mouse motion ----------------

	//if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	//{
	//	int dx = -App->input->GetMouseXMotion();
	//	int dy = -App->input->GetMouseYMotion();

	//	float Sensitivity = 0.25f;

	//	Position -= Reference;

	//	if(dx != 0)
	//	{
	//		float DeltaX = (float)dx * Sensitivity;

	//		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	//		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	//		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	//	}

	//	if(dy != 0)
	//	{
	//		float DeltaY = (float)dy * Sensitivity;

	//		Y = rotate(Y, DeltaY, X);
	//		Z = rotate(Z, DeltaY, X);

	//		if(Y.y < 0.0f)
	//		{
	//			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
	//			Y = cross(Z, X);
	//		}
	//	}

	//	Position = Reference + Z * length(Position);
	//	Position;
	//}
	//if (App->input->GetMouseZ() > 0) {
	//	Position -= Z * speed * 12;
	//}
	//else if (App->input->GetMouseZ() < 0) {
	//	Position += Z * speed * 12;
	//}
	// Recalculate matrix -------------
	//CalculateViewMatrix();

	//return UPDATE_CONTINUE;
}

void ModuleCamera3D::SetPosition(float3 newPosition)
{
	if (currentCamera != nullptr)
	{
		currentCamera->Setposition(newPosition);
	}
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	CalculateViewMatrix();
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

void ModuleCamera3D::SetCurrentCamera(ModuleCamera* newCamera)
{
	currentCamera->isCurrentCamera = false;
	currentCamera = newCamera;
	currentCamera->isCurrentCamera = true;
}

void ModuleCamera3D::SetCullingCamera(ModuleCamera* newCamera)
{
	if (cullingCamera)
		cullingCamera->isCullingCamera = false;

	cullingCamera = newCamera;
	cullingCamera->isCullingCamera = true;
}

void ModuleCamera3D::RaycastSelect()
{
	float2 mousePos = float2(App->input->GetMouseX(), App->window->GetHeight() - App->input->GetMouseY());

	float mouseNormalX = mousePos.x / App->window->GetWidth();
	float mouseNormalY = mousePos.y / App->window->GetHeight();

	mouseNormalX = (mouseNormalX - 0.5) / 0.5;
	mouseNormalY = (mouseNormalY - 0.5) / 0.5;

	LineSegment selectRay = currentCamera->frustum.UnProjectLineSegment(mouseNormalX, mouseNormalY);

	App->renderer3D->DrawLine(selectRay.a, selectRay.b);

	//LOG("Ray goes from: %d to %d", selectRay.a, selectRay.b);

	CheckIntersetions(&selectRay);
}

void ModuleCamera3D::CheckIntersetions(LineSegment* selectRay)
{
	bool objectFound = false;
	std::map<float, GameObject*> hits;

	for (std::vector<GameObject*>::iterator object = App->scene_intro->game_objects.begin(); object != App->scene_intro->game_objects.end(); object++)
	{
		if (selectRay->Intersects((*object)->aabb))
		{
			//LOG("Got a hit with: %s", (*object)->GetName());

			float nearInter, farInter;
			if (selectRay->Intersects((*object)->obb, nearInter, farInter)) //It should intersect 2 times?
			{
				hits.emplace(nearInter, (*object));
				objectFound = true;
			}
		}
	}

	if (objectFound)
		App->scene_intro->SetSelectedObject((*hits.begin()).second);
	//else
		//App->scene->SetSelectedObject(nullptr);

}

void ModuleCamera3D::RotateCameraStatic()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float sensitivity = 0.003f;
	//looking = false;

	// x motion make the camera rotate in Y absolute axis (0,1,0) (not local)
	if (dx != 0.f)
	{
		Quat q = Quat::RotateY(dx * sensitivity);
		currentCamera->frustum.SetFront(q.Mul(currentCamera->frustum.Front()).Normalized());
		currentCamera->frustum.SetUp(q.Mul(currentCamera->frustum.Up()).Normalized());
	}

	// y motion makes the camera rotate in X local axis, with tops
	if (dy != 0.f)
	{
		Quat q = Quat::RotateAxisAngle(currentCamera->frustum.WorldRight(), dy * sensitivity);

		vec newUp = q.Mul(currentCamera->frustum.Up()).Normalized();

		if (newUp.y > 0.0f)
		{
			currentCamera->frustum.SetUp(newUp);
			currentCamera->frustum.SetFront(q.Mul(currentCamera->frustum.Front()).Normalized());
		}
	}
}

