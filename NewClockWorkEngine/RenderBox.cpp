#include "RenderBox.h"
#include "Glew/include/glew.h"
#include "MathGeoLib/include/MathGeoLib.h"

RenderBox::RenderBox(std::vector<float3> points, Color col):bbColor(col),points(points)
{
}

RenderBox::~RenderBox()
{
	points.clear();
	bbColor.Set(0.0f, 0.0f, 0.0f, 0.0f);
}

void RenderBox::Draw()
{
	//there are 8 corners in a box
	// They will be in the order 0: ---, 1: --+, 2: -+-, 3: -++, 4: +--, 5: +-+, 6: ++-, 7: +++.
	//where - is the most negative point and + the max positive.
	glBegin(GL_LINES);

	glLineWidth(2.0f);
	glColor4f(bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	

	//Min lines
	glVertex3f(points[0].x, points[0].y, points[0].z);
	glVertex3f(points[1].x, points[1].y, points[1].z);

	glVertex3f(points[0].x, points[0].y, points[0].z);
	glVertex3f(points[2].x, points[2].y, points[2].z);

	glVertex3f(points[0].x, points[0].y, points[0].z);
	glVertex3f(points[4].x, points[4].y, points[4].z);


	//Max lines
	glVertex3f(points[7].x, points[7].y, points[7].z);
	glVertex3f(points[6].x, points[6].y, points[6].z);

	glVertex3f(points[7].x, points[7].y, points[7].z);
	glVertex3f(points[5].x, points[5].y, points[5].z);

	glVertex3f(points[7].x, points[7].y, points[7].z);
	glVertex3f(points[3].x, points[3].y, points[3].z);

	//Other lines
	//1
	glVertex3f(points[1].x, points[1].y, points[1].z);
	glVertex3f(points[3].x, points[3].y, points[3].z);
	
	glVertex3f(points[1].x, points[1].y, points[1].z);
	glVertex3f(points[5].x, points[5].y, points[5].z);
	//2
	glVertex3f(points[4].x, points[4].y, points[4].z);
	glVertex3f(points[5].x, points[5].y, points[5].z);
	
	glVertex3f(points[4].x, points[4].y, points[4].z);
	glVertex3f(points[6].x, points[6].y, points[6].z);
	//3
	glVertex3f(points[2].x, points[2].y, points[2].z);
	glVertex3f(points[3].x, points[3].y, points[3].z);
	
	glVertex3f(points[2].x, points[2].y, points[2].z);
	glVertex3f(points[6].x, points[6].y, points[6].z);
	
	glEnd();


}
