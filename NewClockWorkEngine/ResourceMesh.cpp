#include<iostream>

#include "ResourceMesh.h"
#include "Resources.h"

#include "Glew/include/glew.h" //order matters
//#include "glmath.h"

#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Application.h"


ResourceMesh::ResourceMesh(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<float> normals, std::vector<float> texCoords, unsigned int UID) :
	Resource(UID, ResourceType::MESH),idVertex(0), idIndex(0), idNormals(0), idTexCoords(0)
{
	this->vertices = vertices;
	this->indices = indices;
	this->normals = normals;
	this->texCoords = texCoords;
	GenerateSmoothedNormals();
	GenerateBuffers();
}

ResourceMesh::ResourceMesh(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<float> normals, std::vector<float> smoothedNormals, std::vector<float> texCoords, unsigned int UID):
	Resource(UID, ResourceType::MESH),idVertex(0), idIndex(0), idNormals(0), idTexCoords(0)
{
	this->vertices = vertices;
	this->indices = indices;
	this->normals = normals;
	this->smoothedNormals = smoothedNormals;
	this->texCoords = texCoords;
	GenerateBuffers();
}

ResourceMesh::ResourceMesh(unsigned int UID) :Resource(UID,ResourceType::MESH), idVertex(0), idIndex(0), idNormals(0), idTexCoords(0)
{
}

ResourceMesh::ResourceMesh(const ResourceMesh& other):Resource(App->renderer3D->seed.Int() ,ResourceType::MESH)
{
	this->vertices = other.vertices;
	this->indices = other.indices;
	this->normals = other.normals;
	this->texCoords = other.texCoords;
	//this->drawMode = other.drawMode;
	//this->normalMode = other.normalMode;
	//this->shadingFlat = other.shadingFlat;
	//this->texture = other.texture;

	this->smoothedNormals = other.smoothedNormals;
	GenerateBuffers();
}

ResourceMesh::~ResourceMesh()
{
	UnloadFromMemory();

}


void ResourceMesh::GenerateSmoothedNormals()
{
	std::vector<float3> faceNormals;//1 normal for every 3 indices
	//first we calculate the mesh face normals
	for (int i = 0; i < indices.size(); i += 3)
	{
		unsigned int vertex0id = indices[i];
		unsigned int vertex1id = indices[i + 1];
		unsigned int vertex2id = indices[i + 2];


		float3 vertex0 = { vertices[vertex0id * 3],vertices[(vertex0id * 3) + 1], vertices[(vertex0id * 3) + 2] };
		float3 vertex1 = { vertices[vertex1id * 3],vertices[(vertex1id * 3) + 1], vertices[(vertex1id * 3) + 2] };
		float3 vertex2 = { vertices[vertex2id * 3], vertices[(vertex2id * 3) + 1], vertices[(vertex2id * 3) + 2] };

		float3 vector01 = vertex1 - vertex0;//vector from point 0 to point 1
		float3 vector02 = vertex2 - vertex0;//vector from point 0 to point 2
		float3 normal = Cross(vector01, vector02);
		normal.Normalize();
		faceNormals.push_back(normal);

	}

	//then we calculate the smoothed averaged normals for each vertex
	smoothedNormals.resize(normals.size());
	for (int i = 0; i < smoothedNormals.size(); i += 3)
	{
		float3 averagedNormal = { 0.0f,0.0f,0.0f };
		std::vector<float3>normalsToAverage;

		for (int j = 0; j < indices.size(); j++)//we iterate every index and save the ones that point to the vertex for which we want to calculate teh normal 
		{
			if (indices[j] == i / 3)
			{
				float3 currFaceNormal = faceNormals[j / 3];

				bool alreadyStored = false;
				for (int l = 0; l < normalsToAverage.size(); l++)
				{
					if (normalsToAverage[l].x == currFaceNormal.x && normalsToAverage[l].y == currFaceNormal.y && normalsToAverage[l].z == currFaceNormal.z)
					{
						alreadyStored = true;
						break;
					}
				}

				if (alreadyStored == false)
					normalsToAverage.push_back(faceNormals[j / 3]);
			}
		}

		//average the normals
		for (int k = 0; k < normalsToAverage.size(); k++)
		{
			averagedNormal += normalsToAverage[k];
		}
		//averagedNormal = averagedNormal / normalsToAverage.size();
		averagedNormal.Normalize();

		smoothedNormals[i] = averagedNormal.x;
		smoothedNormals[i + 1] = averagedNormal.y;
		smoothedNormals[i + 2] = averagedNormal.z;

	}


	
	for (int i = 0; i < vertices.size(); i += 3)//if 2 vertices are in the same place merge their normals
	{
		std::vector<int> normalsToChange;//this is the id of the normals that need to be changed
		std::vector<float3> averagedNormal;

		normalsToChange.push_back(i);
		averagedNormal.push_back({ smoothedNormals[i],smoothedNormals[i + 1],smoothedNormals[i + 2] });

		for (int j = 0; j < vertices.size(); j += 3)
		{
			if (i == j)
				continue;

			if (vertices[i] == vertices[j] && vertices[i + 1] == vertices[j + 1] && vertices[i + 2] == vertices[j + 2])
			{
				normalsToChange.push_back(j);
				averagedNormal.push_back({ smoothedNormals[j],smoothedNormals[j + 1],smoothedNormals[j + 2] });
			}


		}

		if (averagedNormal.size() > 1) //if there is more than one normal to average, average them
		{
			float3 newAverage = {0.0f,0.0f,0.0f};

			for (int i = 0; i < averagedNormal.size(); i++)
			{
				newAverage += averagedNormal[i];
			}
			newAverage.Normalize();

			for (int i = 0; i < normalsToChange.size(); i++)
			{
				smoothedNormals[normalsToChange[i]] = newAverage.x;
				smoothedNormals[normalsToChange[i]+1] = newAverage.y;
				smoothedNormals[normalsToChange[i]+2] = newAverage.z;
			}
		}

	}
}

void ResourceMesh::GenerateBuffers()
{

	//gen buffers=========================================

	//vertex buffer
	glGenBuffers(1, &idVertex);
	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);



	//index buffer
	glGenBuffers(1, &idIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	if (!this->normals.empty())
	{
		//normal buffer
		glGenBuffers(1, &idNormals);
		glBindBuffer(GL_ARRAY_BUFFER, idNormals);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
	}

	if (!this->texCoords.empty())
	{
		//texture coordinate buffer
		glGenBuffers(1, &idTexCoords);
		glBindBuffer(GL_ARRAY_BUFFER, idTexCoords);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
	}

	//clear buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



}

void ResourceMesh::FreeBuffers()
{
	if (idVertex != 0)
	{
		glDeleteBuffers(1, &idVertex);
		idVertex = 0;
	}
	if (idIndex != 0)
	{
		glDeleteBuffers(1, &idIndex);
		idIndex = 0;
	}
	if (idNormals != 0)
	{
		glDeleteBuffers(1, &idNormals);
		idNormals = 0;
	}
	if (idTexCoords != 0)
	{
		glDeleteBuffers(1, &idTexCoords);
		idTexCoords = 0;
	}

}

bool ResourceMesh::UnloadFromMemory()
{
	FreeBuffers();
	indices.clear();
	vertices.clear();
	normals.clear();
	texCoords.clear();
	smoothedNormals.clear();

	isLoaded = false;

	return false;
}

//void Mesh::DrawVertexNormals()
//{
//	float magnitude = 2.0f;
//	glColor3f(1.0f, 0.5f, 0.0f);
//	glLineWidth(4.0f);
//	glBegin(GL_LINES);
//
//
//	for (int i = 0; i < normals.size() / 3; i++)
//	{
//		vec3 vertex0 = { vertices[i * 3], vertices[(i * 3) + 1], vertices[(i * 3) + 2] };
//		vec3 vertex1 =
//		{
//			vertices[i * 3] + (normals[i * 3] * magnitude),
//			vertices[(i * 3) + 1] + (normals[(i * 3) + 1] * magnitude),
//			vertices[(i * 3) + 2] + (normals[(i * 3) + 2] * magnitude)
//		};
//
//
//		glVertex3f(vertex0.x, vertex0.y, vertex0.z);
//		glVertex3f(vertex1.x, vertex1.y, vertex1.z);
//	}
//
//
//
//	glEnd();
//
//	glLineWidth(2.0f);
//
//}
//
//void Mesh::DrawFacesNormals()
//{
//
//
//	float magnitude = 6.0f;
//	glColor3f(0.0f, 0.25f, 1.0f);
//	glLineWidth(4.0f);
//
//
//	glBegin(GL_LINES);
//
//	for (int i = 0; i < indices.size(); i += 3)
//	{
//		unsigned int vertex0id = indices[i];
//		unsigned int vertex1id = indices[i + 1];
//		unsigned int vertex2id = indices[i + 2];
//
//
//		vec3 vertex0 = { vertices[vertex0id * 3], vertices[(vertex0id * 3) + 1], vertices[(vertex0id * 3) + 2] };
//		vec3 vertex1 = { vertices[vertex1id * 3], vertices[(vertex1id * 3) + 1], vertices[(vertex1id * 3) + 2] };
//		vec3 vertex2 = { vertices[vertex2id * 3], vertices[(vertex2id * 3) + 1], vertices[(vertex2id * 3) + 2] };
//
//		vec3 vector01 = vertex1 - vertex0;//vector from point 0 to point 1
//		vec3 vector02 = vertex2 - vertex0;//vector from point 0 to point 2
//		vec3 normal = normalize(cross(vector01, vector02));
//		normal *= magnitude;
//
//
//		vec3 center = (vertex0 + vertex1 + vertex2) / 3;
//		//Provisional placement (it displays the normal in the first vertex)
//		glVertex3f(center.x, center.y, center.z);
//		glVertex3f((center.x + normal.x), (center.y + normal.y), (center.z + normal.z));
//
//	}
//
//	glEnd();
//
//	glLineWidth(2.0f);
//}
//
//void Mesh::DrawBuffers()
//{
//
//	glBindBuffer(GL_ARRAY_BUFFER, idVertex);			//this is for printing the index
//	glVertexPointer(3, GL_FLOAT, 0, NULL);				//Null => somehow OpenGL knows what you're talking about
//	
//	
//	if (shadingFlat && !normals.empty())
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, idNormals);
//		glNormalPointer(GL_FLOAT, 0, NULL);
//	}
//	else idNormals = 0;
//
//	if (!texCoords.empty()) {
//		glBindBuffer(GL_ARRAY_BUFFER, idTexCoords);
//		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
//	}
//	else idTexCoords = 0;
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);	//Because this Bind is after the vertex bind, OpenGl knows these two are in order & connected. *Magic*	
//
//	if(texture!=nullptr&&texture->HasTexture())
//		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
//
//	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);	//End of "bind addition" here...
//	
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}
