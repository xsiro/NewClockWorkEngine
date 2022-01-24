#ifndef __RENDER_AABB__
#define __RENDER_BOX__

#include "MathGeoLib/include/MathGeoLib.h"
#include "Color.h"

class RenderBox
{
public:
	RenderBox(std::vector<float3> points, Color col = Color(1.0f, 1.0f, 1.0f));
	
	~RenderBox();

	void Draw();

private:
	Color bbColor;
	std::vector<float3> points;
};



#endif // !__RENDER_AABB__
