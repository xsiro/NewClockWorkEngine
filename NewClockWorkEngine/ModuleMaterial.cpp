#include "ModuleMaterial.h"
#include "imgui/imgui.h"
#include "Glew/include/glew.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "ResourceMaterial.h"
#include "Application.h"
#include "ModuleResourceManager.h"

ModuleMaterial::ModuleMaterial(GameObject* owner, unsigned int ID, Color color) :Component(ComponentType::MATERIAL, owner, ID),
idCheckers(0), 
usingCkeckers(false), resourceID(0)
{
	matCol = color;
	GenDefaultTexture();
}

ModuleMaterial::~ModuleMaterial()
{
	DestroyCheckers();
	
	usingCkeckers = 0;

	if (resourceID != 0)
	{
		App->rManager->StopUsingResource(resourceID);
		resourceID = 0;
	}
}

//DO NOT use RequestNewResource() method when calling this method, it does it for you
void ModuleMaterial::SetNewResource(unsigned int resourceUID)
{
	if (resourceID != 0)
	{
		App->rManager->StopUsingResource(resourceID);
	}

	Resource* r = App->rManager->RequestNewResource(resourceUID);
	if (r != nullptr)
	{
		resourceID = resourceUID;
	}
	else
	{
		resourceID = 0;
		LOG("[error] the resource with ID:%i, given to this component doesn't exist", resourceUID);
	}
}

unsigned int ModuleMaterial::GetResourceID()
{
	return resourceID;
}

bool ModuleMaterial::HasTexture()
{
	if (resourceID != 0)
	{
		ResourceMaterial* t = (ResourceMaterial*)App->rManager->RequestExistingResource(resourceID);
		if (t == nullptr)
		{
			resourceID = 0;
		}
		if (t != nullptr && t->idTexture != 0)
			return true;
	}
	return false;
}

bool ModuleMaterial::HasCheckers() const
{
	if (idCheckers != 0)
		return true;
	return false;
}

unsigned int ModuleMaterial::GetTextureID() //if 0 then failed to load resource (aka the component ahs no resource associated)
{
	if (resourceID != 0)
	{
		ResourceMaterial* t = (ResourceMaterial*)App->rManager->RequestExistingResource(resourceID);
		if (t == nullptr)
		{
			resourceID = 0;
		}
		else
		{
			return t->GetTextureID();
		}
	}

	return 0;
}

ResourceMaterial* ModuleMaterial::GetTexture()
{
	if (resourceID != 0)
	{
		ResourceMaterial* t = (ResourceMaterial*)App->rManager->RequestExistingResource(resourceID);
		if (t == nullptr)
		{
			resourceID = 0;
		}
		else
		{
			return t;
		}
	}

	return nullptr;
}

unsigned int ModuleMaterial::GetCheckersID() const
{
	return idCheckers;
}

void ModuleMaterial::OnEditor()
{
	bool activeAux = active;

	std::string headerName = "Material";
	std::string suffixLabel = "##Material";
	std::string actualname;
	suffixLabel += std::to_string(ID);
	if (!activeAux)headerName += " (not active)";

	if (!activeAux)ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

	if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!activeAux)ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 0.8f));
		actualname = "IS ACTIVE" + suffixLabel + "Checkbox";
		ImGui::Checkbox(actualname.c_str(), &active); //##adds more to the label id without displaying so we can have 2 checkbox with the same text

		ResourceMaterial* texture = nullptr;
		texture = GetTexture();

		//===========================================

		//get the items here

		std::string textNameDisplay = "No Selected Texture";
		unsigned int myResourceID = 0;
		if (texture != nullptr)
		{
			textNameDisplay = texture->GetName();
			myResourceID = texture->GetUID();
		}

		std::vector<Resource*> allLoadedTextures;
		App->rManager->GetAllResourcesOfType(ResourceType::TEXTURE, allLoadedTextures);
		//const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
		//static int item_current_idx = 0;                    // Here our selection data is an index.
		//const char* combo_label = textNameDisplay.c_str();  // Label to preview before opening the combo (technically it could be anything)
		actualname = "Used Texture" + suffixLabel;
		if (ImGui::BeginCombo(actualname.c_str(), textNameDisplay.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			actualname = "NONE" + suffixLabel;
			const bool noneSelected = (texture == nullptr);
			if (ImGui::Selectable(actualname.c_str(), noneSelected))
			{
				if (texture != nullptr)
				{
					App->rManager->StopUsingResource(resourceID);
					resourceID = 0;
					myResourceID = 0;
					texture == nullptr;

				}
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (noneSelected)
				ImGui::SetItemDefaultFocus();

			//================================
			for (int n = 0; n < allLoadedTextures.size(); n++)
			{
				std::string name = allLoadedTextures[n]->GetName();
				name += suffixLabel;
				name += "List";
				name += std::to_string(n);
				const bool isMatselected = (myResourceID == allLoadedTextures[n]->GetUID());
				if (ImGui::Selectable(name.c_str(), isMatselected))
				{
					SetNewResource(allLoadedTextures[n]->GetUID());
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					std::string count = std::to_string(allLoadedTextures[n]->referenceCount);
					std::string ID = std::to_string(allLoadedTextures[n]->GetUID());
					std::string assetPath = allLoadedTextures[n]->GetAssetFile();
					std::string libPath = allLoadedTextures[n]->GetLibraryFile();

					ImGui::Text("ID: %s", ID.c_str());
					ImGui::Text("References: %s", count.c_str());
					ImGui::Text("Asset Path: %s", assetPath.c_str());
					ImGui::Text("Lib Path: %s", libPath.c_str());

					ImGui::EndTooltip();
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isMatselected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		//===========================================

//begin drag drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("ResourceTex##dragdropSource"))
			{
				IM_ASSERT(payload->DataSize == sizeof(unsigned int));
				unsigned int payloadID = *(const int*)payload->Data;


				if (payloadID != 0 && payloadID != resourceID)
				{
					SetNewResource(payloadID);
				}
			}
			ImGui::EndDragDropTarget();

		}
		//end drag drop target

		ImGui::Indent();
		ImGui::Separator();
		if (texture != nullptr)
		{
			ImGui::Text("ID: %i", this->GetTextureID());
			ImGui::Text("Size in Bytes: %i ", texture->sizeInBytes);
		}
		ImGui::Separator();
		ImGui::Checkbox("Checkers", &usingCkeckers);
		ImGui::Separator();

		if (usingCkeckers || texture != nullptr)
		{
			if (ImGui::TreeNode("Texture Preview"))
			{
				actualname = "Size" + suffixLabel;
				ImGui::SliderInt(actualname.c_str(), &size, 1, 400);
				if (usingCkeckers)
					ImGui::Image((ImTextureID)this->GetCheckersID(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
				else if (texture != nullptr)
					ImGui::Image((ImTextureID)this->GetTextureID(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));

				ImGui::TreePop();
			}
		}
		ImGui::Separator();

		if (ImGui::TreeNode("Select Material Color"))
		{
			ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview;
			actualname = "MyColor" + suffixLabel;
			ImGui::ColorPicker4(actualname.c_str(), (float*)& matCol, flags);
			ImGui::TreePop();
		}

		ImGui::Separator();
		ImGui::Unindent();
		actualname = "Delete Material Component" + suffixLabel;
		if (ImGui::BeginPopup(actualname.c_str(), ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("you are about to delete\n this component");
			actualname = "Go ahead" + suffixLabel;
			if (ImGui::Button(actualname.c_str()))
			{
				toDelete = true;
			}

			ImGui::SameLine();
			actualname = "Cancel" + suffixLabel;
			if (ImGui::Button(actualname.c_str()))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		float maxWidth = ImGui::GetWindowContentRegionMax().x;
		ImGui::SetCursorPosX(maxWidth - 50);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.25f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

		actualname = "Delete" + suffixLabel;
		if (ImGui::Button(actualname.c_str()))
		{
			actualname = "Delete Material Component" + suffixLabel;
			ImGui::OpenPopup(actualname.c_str());
		}


		if (!activeAux)ImGui::PopStyleColor();
		ImGui::PopStyleColor(2);
	}

	if (!activeAux)ImGui::PopStyleColor();

}
//
//void ModuleMaterial::GenTextureFromName(unsigned int ilImageName, std::string path)
//{
//	DestroyTexture();
//
//
//	ilBindImage(ilImageName);
//
//	//get properties
//	ILinfo ImageInfo;
//	iluGetImageInfo(&ImageInfo);
//
//	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
//	{
//		iluFlipImage();
//	}
//
//
//	format = ImageInfo.Format;
//	/*IL_COLOUR_INDEX
//		IL_RGB
//		IL_RGBA
//		IL_BGR
//		IL_BGRA
//		IL_LUMINANCE*/
//
//	depth = ImageInfo.Depth;
//	bpp = ImageInfo.Bpp;//bytes per pixel
//	sizeInBytes = ImageInfo.SizeOfData;//bytes
//	width = ImageInfo.Width;
//	height = ImageInfo.Height;
//
//	//set name
//	this->path = path;
//
//	//gen texture
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glGenTextures(1, &idTexture);
//	glBindTexture(GL_TEXTURE_2D, idTexture);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	ilBindImage(0);
//}
//
//void ModuleMaterial::DestroyTexture()
//{
//	if (idTexture != 0)
//	{
//		glDeleteTextures(1, &idTexture);
//		idTexture = 0;
//	}
//}

void ModuleMaterial::DestroyCheckers()
{
	if (idCheckers != 0)
	{
		glDeleteTextures(1, &idCheckers);
		idCheckers = 0;
	}
}

void ModuleMaterial::GenDefaultTexture()
{
	DestroyCheckers();
	//this->path = "Default texture - no path";
	const int checkersHeight = 100;
	const int checkersWidth = 100;

	GLubyte checkerImage[checkersHeight][checkersWidth][4];
	for (int i = 0; i < checkersWidth; i++) {
		for (int j = 0; j < checkersWidth; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &idCheckers);
	glBindTexture(GL_TEXTURE_2D, idCheckers);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkersWidth, checkersHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glBindTexture(GL_TEXTURE_2D, 0);
}
