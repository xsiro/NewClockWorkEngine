#pragma once
#include<vector>
#include "Resources.h"

enum class MeshDrawMode
{
	DRAW_MODE_BOTH, 
	DRAW_MODE_FILL,
	DRAW_MODE_WIRE
};
enum class NormalDrawMode
{
	NORMAL_MODE_NONE,
	NORMAL_MODE_VERTEX,
	NORMAL_MODE_FACES,
	NORMAL_MODE_BOTH
};

class ResourceMesh : public Resource
{
public:

	ResourceMesh(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<float> normals, std::vector<float> texCoords,unsigned int UID);
	ResourceMesh(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<float> normals, std::vector<float> smoothedNormals, std::vector<float> texCoords, unsigned int UID);
	ResourceMesh(unsigned int UID);
	ResourceMesh(const ResourceMesh& other);
	~ResourceMesh();

	//void Draw();
	void GenerateSmoothedNormals();
	void GenerateBuffers();
	void FreeBuffers();
	bool UnloadFromMemory()override;
private:
	

public:

	unsigned int idIndex; 
	std::vector<unsigned int> indices;

	unsigned int idVertex; 
	std::vector<float> vertices;

	unsigned int idNormals;
	std::vector<float> normals;

	std::vector<float> smoothedNormals;
	unsigned int idTexCoords; 
	std::vector<float> texCoords;

};