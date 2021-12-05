#include "Window.h"


class Win_Inspector : public Window
{
public:
	Win_Inspector(bool _active);
	virtual ~Win_Inspector();
	void Init() override;
	void Draw() override;

	void CleanUp() override;

	bool depthtest;
	bool cullface;
	bool lighting;
	bool polygonssmooth;
	bool colormaterial;
	bool material;
	bool cubemap;
	bool wireframe;
	bool check = false;
	bool vertexlines;
	bool facelines;

};