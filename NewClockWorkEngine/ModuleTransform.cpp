#include "ModuleTransform.h"
#include "imgui.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleComponent.h"

ModuleTransform::ModuleTransform(GameObject* owner) : ModuleComponent(ComponentType::Transform, owner)
{

	position = float3(0.f, 0.f, 0.f);
	positionUI = float3(0.f, 0.f, 0.f);
	scale = float3(1.f, 1.f, 1.f);

	eulerRotationUi = float3(0.f, 0.f, 0.f);
	eulerRotation = float3(0.f, 0.f, 0.f);
	rotation = Quat::identity;

	transform = float4x4::FromTRS(position, rotation, scale);
	globalTransform = Quat::identity;
}

ModuleTransform::~ModuleTransform()
{

}

void ModuleTransform::Update()
{


	//transform.Decompose(position, rotation, scale);
	//RecalculateEuler();
}

void ModuleTransform::CleanUp()
{

}

void ModuleTransform::UpdateTRS()
{
	transform.Decompose(position, rotation, scale);
	RecalculateEuler();
}

void  ModuleTransform::DrawInspector()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		if (ImGui::DragFloat3("Transform", (float*)&position)) { RecalculateMatrix(); };
		if (ImGui::DragFloat3("Scale", (float*)&scale)) { RecalculateMatrix(); };
		if (ImGui::DragFloat3("Rotation", (float*)&eulerRotationUi)) { SetEulerRotation(eulerRotationUi); };
		//if (ImGui::InputFloat3("Transform", (float*)&position, "%.2f", flags)) { RecalculateMatrix(); };
		//if (ImGui::InputFloat3("Scale", (float*)&scale, "%.2f", flags)) { RecalculateMatrix(); };
		//if (ImGui::InputFloat3("Rotation", (float*)&eulerRotationUi, "%.2f")) { SetEulerRotation(eulerRotationUi); }
	
	}
	
}

void ModuleTransform::SetEulerRotation(float3 euler_angles)
{
	float3 delta = (euler_angles - eulerRotation) * 0.0174532925199432957f;
	Quat quaternion_rotation = Quat::FromEulerXYZ(delta.x, delta.y, delta.z);
	rotation = rotation * quaternion_rotation;
	eulerRotation = euler_angles;
	RecalculateMatrix();
}

float4x4 ModuleTransform::GetTransform() const
{
	return transform;
}

float3 ModuleTransform::GetPosition()const
{
	return position;
}

float3 ModuleTransform::GetScale()const
{
	return scale;
}

void ModuleTransform::SetPosition(float3 position)
{
	this->position = position;
	RecalculateMatrix();
}

void ModuleTransform::SetScale(float3 scale)
{
	this->scale = scale;
	RecalculateMatrix();
}

void ModuleTransform::RecalculateMatrix()
{
	transform = float4x4::FromTRS(position, rotation, scale);
}

void ModuleTransform::RecalculateEuler()
{
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation *= 57.295779513082320876f;
}

float4x4 ModuleTransform::GetGlobalTransform()const
{
	return globalTransform;
}