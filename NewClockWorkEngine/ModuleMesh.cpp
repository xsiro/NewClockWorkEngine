#include "Globals.h"
#include "Application.h"
#include "ModuleMesh.h"
#include "imgui.h"
#include "ModuleGui.h"
#include "ModuleImporter.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib") 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew/libx86/glew32.lib")

ModuleMesh::ModuleMesh() : ModuleComponent()
{
	type = ComponentType::Mesh;
	//rendered = false;
}

ModuleMesh::~ModuleMesh()
{
}

static const float g_vertex_buffer_data[] = {
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f };

GLfloat vertices[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front face)
						1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right face)
						1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top face)
					   -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left face)
					   -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom face)
						1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back face)

uint indices[] = { 0, 1, 2,   2, 3, 0,      // front face
				   4, 5, 6,   6, 7, 4,      // right face
				   8, 9,10,  10,11, 8,      // top face
				   12,13,14,  14,15,12,      // left face
				   16,17,18,  18,19,16,      // bottom face
				   20,21,22,  22,23,20 };    // back face

//----------------------------------- pyramid ----------------------------------------
float pyramid_vertices[] = {
	//Bottom face
	0.0f ,0.0f, 0.0f,
	1.0f ,0.0f, 0.0f,
	1.0f ,0.0f, 1.0f,
	0.0f ,0.0f, 1.0f,

	//Top face
	0.5f, 1.0f, 0.5f
};

uint pyramid_indices[]{
	//Bottom face
	0, 1, 2,	2, 3, 0,

	3, 2, 4, // Front face
	0, 3, 4, // Left face
	2, 1, 4, // Right face
	1, 0, 4  // Back face
};

std::vector<GLfloat> sphere_vertices;
std::vector<GLfloat> sphere_normals;
std::vector<GLfloat> sphere_texcoords;
std::vector<GLushort> sphere_indices;

std::vector<GLfloat> cylinder_vertices;
std::vector<GLfloat> cylinder_normals;
std::vector<GLfloat> cylinder_texcoords;
std::vector<GLushort> cylinder_indices;

void ModuleMesh::CreateCubeDirect() {

	//-------------------------------------Cube made by triangles----------------------------------------------//
	glLineWidth(2.0f);
	glDisable(GL_CULL_FACE);
	glBegin(GL_TRIANGLES);

	glVertex3f(1.f, 1.f, 1.f);
	glVertex3f(-1.f, 1.f, 1.f);
	glVertex3f(-1.f, -1.f, 1.f);

	glVertex3f(-1.f, -1.f, 1.f);
	glVertex3f(1.f, -1.f, 1.f);
	glVertex3f(1.f, 1.f, 1.f);

	glVertex3f(1.f, 1.f, 1.f);
	glVertex3f(1.f, -1.f, 1.f);
	glVertex3f(1.f, -1.f, -1.f);

	glVertex3f(1.f, -1.f, -1.f);
	glVertex3f(1.f, 1.f, -1.f);
	glVertex3f(1.f, 1.f, 1.f);

	glVertex3f(1.f, 1.f, 1.f);
	glVertex3f(1.f, 1.f, -1.f);
	glVertex3f(-1.f, 1.f, -1.f);

	glVertex3f(-1.f, 1.f, -1.f);
	glVertex3f(-1.f, 1.f, 1.f);
	glVertex3f(1.f, 1.f, 1.f);

	glVertex3f(-1.f, 1.f, 1.f);
	glVertex3f(-1.f, 1.f, -1.f);
	glVertex3f(-1.f, -1.f, -1.f);

	glVertex3f(-1.f, -1.f, -1.f);
	glVertex3f(-1.f, -1.f, 1.f);
	glVertex3f(-1.f, 1.f, 1.f);

	glVertex3f(-1.f, -1.f, -1.f);
	glVertex3f(1.f, -1.f, -1.f);
	glVertex3f(1.f, -1.f, 1.f);

	glVertex3f(1.f, -1.f, 1.f);
	glVertex3f(-1.f, -1.f, 1.f);
	glVertex3f(-1.f, -1.f, -1.f);

	glVertex3f(1.f, -1.f, -1.f);
	glVertex3f(-1.f, -1.f, -1.f);
	glVertex3f(-1.f, 1.f, -1.f);

	glVertex3f(-1.f, 1.f, -1.f);
	glVertex3f(1.f, 1.f, -1.f);
	glVertex3f(1.f, -1.f, -1.f);
	glEnd();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleMesh::CreateCubeVertex() {

	uint my_id = 0;
	glGenBuffers(1, (GLuint*)&(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data) * 3, g_vertex_buffer_data, GL_STATIC_DRAW);


	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//… bind and use other buffers
	glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data));
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleMesh::CreateCubeIndex() {

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, indices, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleMesh::CreatePyramid() {

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 18, pyramid_indices, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pyramid_vertices);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleMesh::CreateSphere(float radius, unsigned int rings, unsigned int sectors) {

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	sphere_vertices.resize(rings * sectors * 3);
	sphere_normals.resize(rings * sectors * 3);
	sphere_texcoords.resize(rings * sectors * 2);
	std::vector<GLfloat>::iterator v = sphere_vertices.begin();
	std::vector<GLfloat>::iterator n = sphere_normals.begin();
	std::vector<GLfloat>::iterator t = sphere_texcoords.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const y = sin(-M_PI_2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		*t++ = s * S;
		*t++ = r * R;

		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;

		*n++ = x;
		*n++ = y;
		*n++ = z;
	}

	sphere_indices.resize(rings * sectors * 4);
	std::vector<GLushort>::iterator i = sphere_indices.begin();
	for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
		*i++ = (r + 1) * sectors + s;
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = r * sectors + (s + 1);
		*i++ = r * sectors + s;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, &sphere_vertices[0]);
	glNormalPointer(GL_FLOAT, 0, &sphere_normals[0]);
	glTexCoordPointer(2, GL_FLOAT, 0, &sphere_texcoords[0]);
	glDrawElements(GL_QUADS, sphere_indices.size(), GL_UNSIGNED_SHORT, &sphere_indices[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleMesh::CreateCylinder(float radius, float height, int sides) {
	const float PI = 3.1415926f;
	int sectorCount = 36;
	float sectorStep = 2 * PI / sectorCount;
	float sectorAngle;  // radian

	std::vector<float> unitCircleVertices;
	for (int i = 0; i <= sectorCount; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(cos(sectorAngle)); // x
		unitCircleVertices.push_back(sin(sectorAngle)); // y
		unitCircleVertices.push_back(0);                // z
	}

	std::vector<float>().swap(cylinder_vertices);
	std::vector<float>().swap(cylinder_normals);
	std::vector<float>().swap(cylinder_texcoords);

	// get unit circle vectors on XY-plane
	std::vector<float> unitVertices = unitCircleVertices;

	// put side vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			float uz = unitVertices[k + 2];
			// position vector
			cylinder_vertices.push_back(ux * radius);             // vx
			cylinder_vertices.push_back(uy * radius);             // vy
			cylinder_vertices.push_back(h);                       // vz
			// normal vector
			cylinder_normals.push_back(ux);                       // nx
			cylinder_normals.push_back(uy);                       // ny
			cylinder_normals.push_back(uz);                       // nz
			// texture coordinate
			cylinder_texcoords.push_back((float)j / sectorCount); // s
			cylinder_texcoords.push_back(t);                      // t
		}
	}

	// the starting index for the base/top surface
	//NOTE: it is used for generating indices later
	int baseCenterIndex = (int)cylinder_vertices.size() / 3;
	int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

	// put base and top vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float nz = -1 + i * 2;                           // z value of normal; -1 to 1

		// center point
		cylinder_vertices.push_back(0);     cylinder_vertices.push_back(0);     cylinder_vertices.push_back(h);
		cylinder_normals.push_back(0);      cylinder_normals.push_back(0);      cylinder_normals.push_back(nz);
		cylinder_texcoords.push_back(0.5f); cylinder_texcoords.push_back(0.5f);

		for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			// position vector
			cylinder_vertices.push_back(ux * radius);             // vx
			cylinder_vertices.push_back(uy * radius);             // vy
			cylinder_vertices.push_back(h);                       // vz
			// normal vector
			cylinder_normals.push_back(0);                        // nx
			cylinder_normals.push_back(0);                        // ny
			cylinder_normals.push_back(nz);                       // nz
			// texture coordinate
			cylinder_texcoords.push_back(-ux * 0.5f + 0.5f);      // s
			cylinder_texcoords.push_back(-uy * 0.5f + 0.5f);      // t
		}
	}

	std::vector<int> indices;
	int k1 = 0;                         // 1st vertex index at base
	int k2 = sectorCount + 1;           // 1st vertex index at top

	// indices for the side surface
	for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
	{
		// 2 triangles per sector
		// k1 => k1+1 => k2
		cylinder_indices.push_back(k1);
		cylinder_indices.push_back(k1 + 1);
		cylinder_indices.push_back(k2);

		// k2 => k1+1 => k2+1
		cylinder_indices.push_back(k2);
		cylinder_indices.push_back(k1 + 1);
		cylinder_indices.push_back(k2 + 1);
	}

	// indices for the base surface
	//NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
	//      please see the previous code snippet
	for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			cylinder_indices.push_back(baseCenterIndex);
			cylinder_indices.push_back(k + 1);
			cylinder_indices.push_back(k);
		}
		else // last triangle
		{
			cylinder_indices.push_back(baseCenterIndex);
			cylinder_indices.push_back(baseCenterIndex + 1);
			cylinder_indices.push_back(k);
		}
	}

	// indices for the top surface
	for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			cylinder_indices.push_back(topCenterIndex);
			cylinder_indices.push_back(k);
			cylinder_indices.push_back(k + 1);
		}
		else // last triangle
		{
			cylinder_indices.push_back(topCenterIndex);
			cylinder_indices.push_back(k);
			cylinder_indices.push_back(topCenterIndex + 1);
		}
	}

	glRotatef(90, 1, 0, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, &cylinder_vertices[0]);
	glNormalPointer(GL_FLOAT, 0, &cylinder_normals[0]);
	glTexCoordPointer(2, GL_FLOAT, 0, &cylinder_texcoords[0]);
	glDrawElements(GL_TRIANGLES, cylinder_indices.size(), GL_UNSIGNED_SHORT, &cylinder_indices[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
