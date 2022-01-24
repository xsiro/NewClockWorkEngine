#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

//#include "glmath.h" //it has to be incuded here because of the Identity Matrix
//#include "Color.h"
//#include "Glew/include/glew.h"
#include <vector>


enum class PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,

	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Cone, //cyl
	Primitive_Box,	//cyl
	Primitive_Unknown
};

//Related functions to primitives, but we don't want them to be part of the class
//=============================================

void SphereFillVectorsVertexAndIndex(std::vector<float> &vertices, std::vector<unsigned int> &index, float radius = 1.f, unsigned int sectors = 36, unsigned int stacks = 18);
void CylinderFillVectorsVertexAndIndex(std::vector<float>& vertices, std::vector<unsigned int>& index, float rBase = 1, float rTop = 1, float height = 2, unsigned int sectors = 36, unsigned int stacks = 8);
void ConeFillVectorsVertexAndIndex(std::vector<float>& vertices, std::vector<unsigned int>& indices, float rBase = 1, float height = 2, unsigned int sectors = 36, unsigned int stacks = 8);
// ============================================

#endif // !__PRIMITIVES_H__

