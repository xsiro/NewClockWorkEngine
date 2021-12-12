#include "ModuleComponent.h"
#include "Config.h"
#include "Application.h"
#include "GameObject.h"

#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"

#include "ModuleCamera.h"
#include "ModuleTransform.h"

//#include "Dependecies/mmgr/mmgr.h"

ModuleCamera::ModuleCamera(GameObject* owner) : ModuleComponent(ComponentType::Camera, owner)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	frustum.SetViewPlaneDistances(0.1f, 1000.0f);
	frustum.SetPerspective(1.0f, 1.0f);

	SetAspectRatio((float)App->window->GetWidth() / (float)App->window->GetHeight());
	SetVerticalFov(50.0f);

	UpdatePlanes();

	corners = new vec[8];
}

void ModuleCamera::Update()
{
	if (!isCurrentCamera)
	{
		frustum.GetCornerPoints(corners);
		App->renderer3D->DrawBox(corners);
	}

	//frustum.ComputeViewMatrix();
}

void ModuleCamera::CleanUp()
{
	delete[] corners;
}

//void ModuleCamera::Serialize(JsonNode* node)
//{
//	node->AddBool("Is Current Camera", isCurrentCamera);
//	node->AddBool("Is Culling Camera", isCullingCamera);
//}
//
//void ModuleCamera::Load(JsonNode* node)
//{
//	if (node->GetBool("Is Current Camera"))
//		App->camera->SetCurrentCamera(this);
//	if (node->GetBool("Is Culling Camera"))
//		App->camera->SetCullingCamera(this);
//}

float* ModuleCamera::GetViewMatrix()
{
	static float4x4 m;

	m = frustum.ViewMatrix();

	m.Transpose();

	return (float*)m.v;
}

void ModuleCamera::OnUpdateTransform(float4x4 globalTransform)
{
	/*frustum.SetFront(owner->transform->GetGlobalTransform().WorldZ());
	frustum.SetUp(owner->transform->GetGlobalTransform().WorldY());

	float3 position, scale = float3::zero;
	Quat rotation = Quat::identity;
	owner->transform->GetGlobalTransform().Decompose(position, rotation, scale);

	frustum.SetPos(position);
	UpdatePlanes();*/
}

void ModuleCamera::UpdatePlanes()
{
	frustum.GetPlanes(planes);
}

void ModuleCamera::Setposition(float3 pos)
{
	frustum.SetPos(pos);
	UpdatePlanes();
}

void ModuleCamera::SetNearPlane(float distance)
{
	frustum.SetViewPlaneDistances(distance, GetFarPlaneDistance());
	UpdatePlanes();
}

void ModuleCamera::SetFarPlane(float distance)
{
	frustum.SetViewPlaneDistances(GetNearPlaneDistance(), distance);
	UpdatePlanes();
}

void ModuleCamera::SetVerticalFov(float verticalFov) //fov
{
	frustum.SetVerticalFovAndAspectRatio(verticalFov * DEGTORAD, ((float)App->window->GetWidth() / (float)App->window->GetHeight())); //win width / win height
}

void ModuleCamera::SetHorizontalFov(float horizontalFov)
{
	frustum.SetHorizontalFovAndAspectRatio(horizontalFov, ((float)App->window->GetWidth() / (float)App->window->GetHeight()));
}

void ModuleCamera::SetAspectRatio(float ratio)
{
	frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), ratio);
}

float ModuleCamera::ComputeAspectRatio(float verticalFov, float horizontalFov)
{
	return (Tan(verticalFov / 2) / Tan(horizontalFov / 2));
}

Frustum ModuleCamera::GetFrustum() const
{
	return frustum;
}

float3 ModuleCamera::GetPos()const
{
	return frustum.Pos();
}

float ModuleCamera::GetNearPlaneDistance()const
{
	return frustum.NearPlaneDistance();
}

float ModuleCamera::GetFarPlaneDistance()const
{
	return frustum.FarPlaneDistance();
}

float ModuleCamera::GetVerticalFov()const
{
	return frustum.VerticalFov() * RADTODEG;
}

float ModuleCamera::GetHorizontalFov()const
{
	return frustum.HorizontalFov();
}

void ModuleCamera::DrawInspector()
{
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;

	if (ImGui::CollapsingHeader("Camera"))
	{
		float3 pos = this->GetPos();
		if (ImGui::DragFloat3("Camera Position", (float*)&pos, 0.5))
		{
			this->Setposition(pos);
		}

		float nearPlane = this->GetNearPlaneDistance();
		if (ImGui::DragFloat("Near Plane Distance", &nearPlane, 0.5f, 0.f))
		{
			this->SetNearPlane(nearPlane);
		}

		float farPlane = this->GetFarPlaneDistance();
		if (ImGui::DragFloat("Far Plane Distance", &farPlane, 0.5f, 0.1f))
		{
			this->SetFarPlane(farPlane);
		}

		float verticalFov = this->GetVerticalFov();
		if (ImGui::DragFloat("Vertical Fov", &verticalFov, 0.1f, 30.f, 120.f))
		{
			this->SetVerticalFov(verticalFov);
		}

		ImGui::Checkbox("Culling", &this->cull);
		if (ImGui::Button("Set As Current Camera"))
		{
			App->camera->SetCurrentCamera(this);
		}

		if (ImGui::Button("Set As Culling Camera"))
		{
			App->camera->SetCullingCamera(this);
		}
	}
}