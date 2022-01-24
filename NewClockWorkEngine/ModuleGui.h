#pragma once

#include "Module.h"
//#include "Globals.h"
//#include "glmath.h"
#include "imgui/imgui.h" //ImVec2 cannot be forward declared
//#include "Primitive.h" //wtf
//#include <iostream> 
#include <vector> 
#include <string>

class Cnsl;
enum class PrimitiveTypes;


class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Init();
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();
	void CreateDockingSpace();
	void OnResize(int width, int height);
	void Draw();

	void ShowExampleAppConsole(bool* p_open);
	bool Show3DWindow();
	void GuizmoEditTransform();
	bool showAboutWindow();
	bool showConfigFunc();
	bool showQuitPopup();
	bool ShowResourcesActive();

	void DrawDirectoryTree(const char* dir);
	void DrawDirectoryTreeLoadScene(const char* dir);

private:

	bool CreateBasicForm(PrimitiveTypes type, float ar1 = 0, float ar2 = 0, float ar3 = 0, float ar4 = 0, float ar = 5); //arX meaning depends on type. This saves massive amounts of code. Deal with it.
	void CreateMeshfromPrimAndSendToScene(std::vector<float> vertices, std::vector<unsigned int> indices, std::string name = "");
	void OpenGLOnResize(int w, int h);

public:
	bool showDemoWindow;
	bool showConsoleWindow;
	bool showHierarchy;
	bool showLoadFileWindow;
	bool showLoadScenes;
	bool showInspector;
	bool show3DWindow;
	bool showAboutWindowbool;
	bool showConfig;
	bool showQuit;
	bool quitAlreadyOpened;
	bool showResourcesActive;
	Cnsl* console;

	void GetViewportRectUI(float2& screenPos, float2& size)const;

private:
	//arguments for the primitive functions (TODO this could be changed in the future, find a way to have multiple args)
	float ar1;
	float ar2;
	float ar3;
	float ar4;
	float ar5;

	//fps related business
	std::vector<float> fps_log;
	int maxFPShown;

	//flags
	bool Vsync;
	bool resizable;
	bool showOnlyLoadedRes;
	ImVec2 imgSize;
	ImVec2 imgPos;
	float gizmoSize;



	//File explorer selected file  
	char selectedFile[250];
	char selectedScene[250];

};
