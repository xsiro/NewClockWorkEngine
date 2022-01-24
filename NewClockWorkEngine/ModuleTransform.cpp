
#include "GameObject.h"
#include "ModuleTransform.h"
#include "imgui/imgui.h" 
#include "Assimp/include/matrix4x4.inl"
#include "Assimp/include/vector3.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "Globals.h" 


ModuleTransform::ModuleTransform(GameObject* owner, float4x4 lTransform, unsigned int ID, bool isLocalTrans) :Component(ComponentType::TRANSFORM, owner, ID), localMode(true)
{
	if (isLocalTrans)
	{
		lTransformMat = lTransform;
		gTransformMat = float4x4::identity;
		UpdateGlobalMat();

	}
	else
	{
		lTransformMat = float4x4::identity;
		gTransformMat = lTransform;
		UpdateLocalMat();
	}
}

ModuleTransform::~ModuleTransform()
{
}

float4x4 ModuleTransform::GetGlobalTransform() const
{
	return gTransformMat;
}

float4x4 ModuleTransform::GetLocalTransform() const
{
	return lTransformMat;
}

void ModuleTransform::SetGlobalTransform(float4x4 newGTransform)
{
	gTransformMat = newGTransform;
	UpdateLocalMat();
	owner->UpdateChildTransforms();
}

void ModuleTransform::SetLocalTransform(float4x4 newLTransform)
{
	lTransformMat = newLTransform;
	UpdateGlobalMat();
	owner->UpdateChildTransforms();
}

void ModuleTransform::OnEditor()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();
		ImGui::Separator();

		if (ImGui::RadioButton("LocalMode", localMode))
		{
			localMode = true;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("GlobalMode", !localMode))
		{
			localMode = false;
		}
		ImGui::Spacing();

		float v[3];
		float3 vAux;

		if (localMode == true)
		{

			vAux = lTransformMat.TranslatePart();
			v[0] = vAux.x;
			v[1] = vAux.y;
			v[2] = vAux.z;

			if (ImGui::DragFloat3("Position##lPos", v, 0.1f))
			{
				SetLocalPosition(float3(v[0], v[1], v[2]));
			}
			ImGui::Separator();

	
			float3 euler = lTransformMat.ToEulerXYZ();
			float4x4 auxTrans = float4x4::FromEulerXYZ(euler.x, euler.y, euler.z);
			auxTrans.Inverse();

			v[0] = RadToDeg(euler.x);
			v[1] = RadToDeg(euler.y);
			v[2] = RadToDeg(euler.z);

			if (ImGui::DragFloat3("Rotation##lRot", v, 0.01f, -360.0f, 360.0f))
			{
		
				Quat q = Quat::FromEulerXYZ(DegToRad(v[0]), DegToRad(v[1]), DegToRad(v[2]));
				lTransformMat = lTransformMat * auxTrans;
				lTransformMat = lTransformMat * q;
				UpdateGlobalMat();
				owner->UpdateChildTransforms();

			}
			ImGui::Separator();


			vAux = GetLocalScale();
			v[0] = vAux.x;
			v[1] = vAux.y;
			v[2] = vAux.z;

			if (ImGui::DragFloat3("Scale##lScale", v, 0.1f, 0.01f, 1000.0f))
			{

				SetLocalScale(float3(v[0], v[1], v[2]));
			}
		}
		else
		{
			vAux = gTransformMat.TranslatePart();
			v[0] = vAux.x;
			v[1] = vAux.y;
			v[2] = vAux.z;

			if (ImGui::DragFloat3("Position##gPos", v, 0.1f))
			{
				SetGlobalPosition(float3(v[0], v[1], v[2]));
			}
			ImGui::Separator();

	
			float3 euler = gTransformMat.ToEulerXYZ();
			float4x4 auxTrans = float4x4::FromEulerXYZ(euler.x, euler.y, euler.z);
			auxTrans.Inverse();

			v[0] = RadToDeg(euler.x);
			v[1] = RadToDeg(euler.y);
			v[2] = RadToDeg(euler.z);

			if (ImGui::DragFloat3("Rotation##gRot", v, 0.01f, -360.0f, 360.0f))
			{
				Quat q = Quat::FromEulerXYZ(DegToRad(v[0]), DegToRad(v[1]), DegToRad(v[2]));
				gTransformMat = gTransformMat * auxTrans;
				gTransformMat = gTransformMat * q;
				UpdateLocalMat();
				owner->UpdateChildTransforms();

			}
			ImGui::Separator();


			vAux = GetGlobalScale();
			v[0] = vAux.x;
			v[1] = vAux.y;
			v[2] = vAux.z;

			if (ImGui::DragFloat3("Scale##gScale", v, 0.1f, 0.01f, 1000.0f))
			{

				SetGlobalScale(float3(v[0], v[1], v[2]));
			}
		}

		ImGui::Separator();


		if (ImGui::TreeNode("Local Matrix")) {

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; j++)
				{
					float auxF = float(lTransformMat.v[i][j]);
					ImGui::Text("%f", auxF);
					if (j < 3) ImGui::SameLine();
				}
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}


		ImGui::Separator();
		if (ImGui::TreeNode("Global Matrix")) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; j++)
				{
					float auxF = float(gTransformMat.v[i][j]);
					ImGui::Text("%f", auxF);
					if (j < 3) ImGui::SameLine();
				}
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}

		ImGui::Separator();
		ImGui::Unindent();
	}
}

float3 ModuleTransform::GetLocalPosition()
{
	return lTransformMat.TranslatePart();
}

float3 ModuleTransform::GetGlobalPosition()
{
	return gTransformMat.TranslatePart();
}

float3 ModuleTransform::GetLocalScale()
{
	return lTransformMat.GetScale();
}

float3 ModuleTransform::GetGlobalScale()
{
	return gTransformMat.GetScale();
}


void ModuleTransform::SetLocalPosition(float3 newPos)
{
	lTransformMat.SetTranslatePart(newPos);
	UpdateGlobalMat();
	owner->UpdateChildTransforms();
}

void ModuleTransform::SetLocalScale(float3 newScale)
{

	float3 scale = lTransformMat.GetScale();
	float3 auxScale;
	auxScale.x = (scale.x == 0.0f) ? newScale.x : (newScale.x / scale.x);
	auxScale.y = (scale.y == 0.0f) ? newScale.y : (newScale.y / scale.y);
	auxScale.z = (scale.z == 0.0f) ? newScale.z : (newScale.z / scale.z);
	float ret = auxScale.x * auxScale.y * auxScale.z;
	if (ret < 0) LOG("[error] scale is 0");

	lTransformMat = lTransformMat * float4x4::Scale(auxScale);

	UpdateGlobalMat();
	owner->UpdateChildTransforms();

}

void ModuleTransform::SetLocalRot(Quat newRot)
{


	UpdateGlobalMat();
	owner->UpdateChildTransforms();
}

void ModuleTransform::SetGlobalPosition(float3 newPos)
{
	gTransformMat.SetTranslatePart(newPos);
	UpdateLocalMat();
	owner->UpdateChildTransforms();
}

void ModuleTransform::SetGlobalScale(float3 newScale)
{
	float3 scale = gTransformMat.GetScale();
	float3 auxScale;
	auxScale.x = (scale.x == 0.0f) ? newScale.x : (newScale.x / scale.x);
	auxScale.y = (scale.y == 0.0f) ? newScale.y : (newScale.y / scale.y);
	auxScale.z = (scale.z == 0.0f) ? newScale.z : (newScale.z / scale.z);
	float ret = auxScale.x * auxScale.y * auxScale.z;
	if (ret < 0) LOG("[error] scale is 0");
	gTransformMat = gTransformMat * float4x4::Scale(auxScale);

	UpdateLocalMat();
	owner->UpdateChildTransforms();
}

void ModuleTransform::SetGlobalRot(Quat newRot)
{


	UpdateLocalMat();
	owner->UpdateChildTransforms();
}

void ModuleTransform::UpdateGlobalMat()
{
	if (owner->parent != nullptr)
	{
		gTransformMat = owner->parent->GetComponent<ModuleTransform>()->GetGlobalTransform() * lTransformMat;
	}
	else
		gTransformMat = lTransformMat;
}

void ModuleTransform::UpdateLocalMat()
{
	if (owner->parent != nullptr)
	{
		lTransformMat = owner->parent->GetComponent<ModuleTransform>()->GetGlobalTransform().Inverted() * gTransformMat;
	}
	else
		lTransformMat = gTransformMat;
}



