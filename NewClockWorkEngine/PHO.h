#pragma once
#include <string>
#include <vector>
enum class PHOType 
{
	// FOLDER,
	MESH,
	TEXTURE,
	MATERIAL,
	MODEL,
	SCENE,
	UNKNOWN, 
};

struct PHO
{
	unsigned __int64 ID = 0;
	PHOType type = PHOType::UNKNOWN;

	std::string name = "";
	std::string assetPath = "";
	std::string libraryPath = "";

	//std::vector<unsigned __int64> containedPHOs;

	PHO() {}; //Looks like we need default constructor for maps
	PHO(PHOType type, const char* file, const char* name, unsigned __int64 id) : type(type), assetPath(file), name(name ? name : ""), ID(id) {};

	bool Compare(const char* file, const char* name, PHOType type) const
	{
		return (this->assetPath == file && (name ? this->name == name : true) && (type != PHOType::UNKNOWN ? type == this->type : true));
	}

	/*
	void Serialize(Config& config) const
	{
		config.SetNumber("ID", ID);
		config.SetString("Name", name.c_str());
		config.SetNumber("Type", static_cast<int>(type));
		config.SetString("Library file", libraryFile.c_str());
	}
	*/
};
