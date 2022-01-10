#include "ModuleTransform.h"
#include "imgui/include/imgui.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleComponent.h"
#include "Config.h"

ModuleTransform::ModuleTransform(GameObject* owner) : ModuleComponent(ComponentType::Transform, owner)
{

	position = float3(0.f, 0.f, 0.f);
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

	if (updateTransform)
	{
		owner->UpdatedTransform();
	}

	if (updateTransform)
	{
		LOG("UpdateTransform still true");
	}
	
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

		if (ImGui::DragFloat3("Transform", (float*)&position)) { UpdateLocalTransform(); };
		if (ImGui::DragFloat3("Scale", (float*)&scale)) { UpdateLocalTransform(); };
		if (ImGui::DragFloat3("Rotation", (float*)&eulerRotationUi)) { SetEulerRotation(eulerRotationUi); };
	}
	
}

void ModuleTransform::SetEulerRotation(float3 euler_angles)
{
	float3 delta = (euler_angles - eulerRotation) * 0.0174532925199432957f;
	Quat quaternion_rotation = Quat::FromEulerXYZ(delta.x, delta.y, delta.z);
	rotation = rotation * quaternion_rotation;
	eulerRotation = euler_angles;
	UpdateLocalTransform();
}

void ModuleTransform::OnSave(ConfigNode* node)
{
	ConfigArray _position = node->InitArray("Position");
	_position.AddNumber(position.x);
	_position.AddNumber(position.y);
	_position.AddNumber(position.z);

	ConfigArray _scale = node->InitArray("Scale");
	_scale.AddNumber(scale.x);
	_scale.AddNumber(scale.y);
	_scale.AddNumber(scale.z);

	ConfigArray _rotation = node->InitArray("Rotation");
	_rotation.AddNumber(rotation.x);
	_rotation.AddNumber(rotation.y);
	_rotation.AddNumber(rotation.z);
	_rotation.AddNumber(rotation.w);
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
	UpdateLocalTransform();
}

void ModuleTransform::SetScale(float3 scale)
{
	this->scale = scale;
	UpdateLocalTransform();
}

void ModuleTransform::SetLocalTransform(float3 position, float3 scale, Quat rotation)
{
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;
	UpdateLocalTransform();
}

void ModuleTransform::SetTransform(float3 position, float3 scale, Quat rotation)
{

}

void ModuleTransform::RecalculateMatrix()
{
	transform = float4x4::FromTRS(position, rotation, scale);
}

void ModuleTransform::RecalculateEuler()
{
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation *= 57.295779513082320876f;
	eulerRotationUi = eulerRotation;
}

float4x4 ModuleTransform::GetGlobalTransform()const
{
	return globalTransform;
}


void ModuleTransform::UpdatedTransform(float4x4 parentGlobalTransform)
{
	globalTransform = parentGlobalTransform * transform;
	UpdateTRS();
	LOG("Updated Transform of: %s", owner->GetName());
	updateTransform = false;
}

void ModuleTransform::UpdateLocalTransform()
{
	transform = float4x4::FromTRS(position, rotation, scale);
	updateTransform = true;
	RecalculateEuler();
}

