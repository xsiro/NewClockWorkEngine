
//#include "Globals.h"
//#include "Glew/include/glew.h" //order matters
//#include <gl/GL.h>
//#include <gl/GLU.h>
#include "Primitive.h"
#include "Application.h" //This could be erased from here? (only used for PI)
//#include "glmath.h"
//#include "MathGeoLib/include/MathGeoLib.h"


// ------------------------------------------------------------
void SphereFillVectorsVertexAndIndex(std::vector<float> &vertices, std::vector<unsigned int> &indices, float radius, unsigned int sectors, unsigned int stacks)
{
		float x, y, z, xy;
		int k1, k2;

		// vertex position

		float sectorStep = 2 * pi / sectors;
		float stackStep = pi / stacks;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stacks; ++i)
		{
			stackAngle = pi / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = radius * cosf(stackAngle);             // r * cos(u)
			z = radius * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectors; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
			}
		}


		// generate CCW index list of sphere triangles
		for (int i = 0; i < stacks; ++i)
		{
			k1 = i * (sectors + 1);     // beginning of current stack
			k2 = k1 + sectors + 1;      // beginning of next stack

			for (int j = 0; j < sectors; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stacks - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}
}

void CylinderFillVectorsVertexAndIndex(std::vector<float>& vertices, std::vector<unsigned int>& indices, float rBase, float rTop, float height, unsigned int sectorCount, unsigned int stacks)
{
	std::vector<float> unitCircleVertices; // this will be used in filling the vertices for the base and top

	float sectorStep = 2 * pi / sectorCount;
	float stackStep = pi / stacks;
	float sectorAngle, stackAngle;
	float radius;                     // radius for each stack
	float x, y, z;                     // vertex position
	// generate vertices for a cylinder



	for (int i = 0; i <= sectorCount; ++i) //filling spaces in a unit cirlce
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(cos(sectorAngle)); // x
		unitCircleVertices.push_back(sin(sectorAngle)); // y
		unitCircleVertices.push_back(0);                // z
	}

	// put side vertices to arrays
	for (int i = 0; i <= stacks; ++i)
	{
		z = -(height * 0.5f) + (float)i / stacks * height;      // vertex position z
		radius = rBase + (float)i / stacks * (rTop - rBase);     // lerp
		float t = 1.0f - (float)i / stacks;   // top-to-bottom

		for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			x = unitCircleVertices[k];
			y = unitCircleVertices[k + 1];
			vertices.push_back(x * radius);
			vertices.push_back(y * radius);
			vertices.push_back(z);
		}
	}
	// remember where the base.top vertices start
	unsigned int baseCenterIndex = (unsigned int)vertices.size() / 3;

	// put vertices of base of cylinder
	z = -height * 0.5f;
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(z);

	for (int i = 0, j = 0; i < sectorCount; ++i, j += 3)
	{
		x = unitCircleVertices[j];
		y = unitCircleVertices[j + 1];
		vertices.push_back(x * rBase);
		vertices.push_back(y * rBase);
		vertices.push_back(z);
	}

	// remember where the base vertices start
	unsigned int topCenterIndex = (unsigned int)vertices.size() / 3;

	// put vertices of top of cylinder
	z = height * 0.5f;
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(z);

	for (int i = 0, j = 0; i < sectorCount; ++i, j += 3)
	{
		x = unitCircleVertices[j];
		y = unitCircleVertices[j + 1];
		vertices.push_back(x * rTop);
		vertices.push_back(y * rTop);
		vertices.push_back(z);
	}

	// generate CCW index list of cylinder triangles
	uint k1, k2;

	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectorCount + 1);     // bebinning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 trianles per sector

			indices.push_back(k1);
			indices.push_back(k1 + 1);
			indices.push_back(k2);

			indices.push_back(k2);
			indices.push_back(k1 + 1);
			indices.push_back(k2 + 1);

		}
	}


	// remember where the base indices start
	//uint baseIndex = (unsigned int)indices.size();

	// put indices for base
	for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < (sectorCount - 1))
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(k + 1);
			indices.push_back(k);
		}
		else // last triangle
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(baseCenterIndex + 1);
			indices.push_back(k);
		}
	}

	// remember where the base indices start

	for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < (sectorCount - 1)) {
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(k + 1);
		}

		else {
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(topCenterIndex + 1);
		}
	}
}

void ConeFillVectorsVertexAndIndex(std::vector<float>& vertices, std::vector<unsigned int>& indices, float rBase, float height, unsigned int sectors, unsigned int stacks)
{
	CylinderFillVectorsVertexAndIndex(vertices, indices, rBase, 0, height, sectors, stacks); //Megamind
}


// ------------------------------------------------------------


