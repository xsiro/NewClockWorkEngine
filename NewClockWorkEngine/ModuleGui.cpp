
//The order is relevant from here...
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ImGuizmo/ImGuizmo.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL.h"

//...to here
#include "Application.h"
#include "Globals.h"
#include "ModuleGUI.h"
//#include "ModuleRenderer3D.h" //we need the projection matrix
//#include "ModuleCamera3D.h"


#include "Primitive.h"
//We're using pretty much all of it for cheks, so we're just including the whole thing
#include "MathGeoLib/include/MathGeoLib.h"

#include "Console.h"
#include "MathChecks.h"
//#include "Mesh.h"
#include "ModuleWindow.h"
#include "Hierarchy.h"
#include "GameObject.h"
#include "Importer.h"
#include "Component.h"
#include "ModuleMesh.h"
#include "ResourceMesh.h"
#include "ModuleMaterial.h"
#include "ModuleTransform.h"

//We're using pretty much all of it for cheks, so we're just including the whole thing
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#include<algorithm>


ModuleGUI::ModuleGUI(bool start_enabled) :console(nullptr)
{
	showDemoWindow = false;
	showConsoleWindow = true;
	showHierarchy = true;
	showLoadFileWindow = true;
	showLoadScenes = false;
	showInspector = true;
	show3DWindow = true;
	showAboutWindowbool = false;
	showConfig = false;
	showQuit = false;
	quitAlreadyOpened = false;
	showResourcesActive = true;
	maxFPShown = 60;

	Vsync = VSYNC;

	resizable = true;


	ar1 = 0;
	ar2 = 0;
	ar3 = 0;
	ar4 = 0;
	ar5 = 0;

	console = new Cnsl();
	imgPos = ImVec2(0.0f, 0.0f);
	imgSize = ImVec2(0.0f, 0.0f);

	gizmoSize = 1.0f;
	showOnlyLoadedRes = true;

	selectedFile[0] = '\0';
	selectedScene[0] = '\0';
}

ModuleGUI::~ModuleGUI()
{
	if (console != nullptr)
	{
		delete(console);
		console = nullptr;
	}

}

bool ModuleGUI::Init()
{
	bool ret = true;



	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 3.0f;
		style.GrabRounding = style.FrameRounding = 2.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 140"); //this is hardcoded. Deal with it.

	//fps goodness
	fps_log.resize(maxFPShown);


	

	return ret;
}

update_status ModuleGUI::PreUpdate(float dt)
{
	update_status status = UPDATE_CONTINUE;


	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	//Start ImGuizmo frame
	ImGuizmo::BeginFrame();

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	CreateDockingSpace();


	//Main Menu bar test
	if (ImGui::BeginMainMenuBar())
	{

		if (ImGui::BeginMenu("Menu", true))
		{
			if (ImGui::MenuItem("Save", "Scene")) {

				LOG("Saving absolute scene"); char* buff = "";
				unsigned int size = Importer::SerializeScene(App->scene->root, &buff);
				std::string auxPath = LIB_PATH; auxPath += "Scenes/AbsoluteScene"; auxPath += std::to_string(App->renderer3D->seed.Int()); auxPath += ".pho";
				App->fileSystem->SavePHO(auxPath.c_str(), buff, size);
				RELEASE_ARRAY(buff);
				buff = nullptr;

			}
			if (ImGui::MenuItem("Load", "Scene"))
			{
				showLoadScenes = true;
			}

			if (ImGui::MenuItem("About", "...")) {
				ImGui::SetNextWindowSize(ImVec2(435, 800));
				showAboutWindowbool = true;
			}
			if (ImGui::MenuItem("Config")) {
				//ImGui::SetNextWindowSize(ImVec2(435, 800));
				showConfig = true;
			}

			if (ImGui::MenuItem("Exit", "Esc"))
			{
				showQuit = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObjects##menu", true))
		{
			GameObject* selectedOnSpawn = nullptr;

			if (ImGui::MenuItem("Empty##GameObjectCreate"))
			{
				selectedOnSpawn = new GameObject(App->scene->root, "Empty", float4x4::identity);
			}
			if (ImGui::MenuItem("Child Empty##GameObjectCreate"))
			{
				GameObject* root = App->scene->root;
				if (!App->scene->selectedGameObjs.empty())
				{
					root = App->scene->selectedGameObjs.back();
				}
				selectedOnSpawn = new GameObject(root, "Child Empty", float4x4::identity);
			}
			if (ImGui::MenuItem("Camera##GameObjectCreate"))
			{
				selectedOnSpawn = new GameObject(App->scene->root, "Camera", float4x4::identity);
				selectedOnSpawn->CreateComponent(ComponentType::CAMERA);
				
			}



			if (selectedOnSpawn != nullptr)
			{
				App->scene->SetSelectedGameObject(selectedOnSpawn);
				selectedOnSpawn = nullptr;
			}
			

			ImGui::EndMenu();
		}
		//Basic forms menu (TODO it is disabled for the moment)
		if (ImGui::BeginMenu("Basic Forms Generator", false))
		{

			//Spheres
			if (ImGui::BeginMenu("Spheres")) {
				ImGui::Text("Sphere param:");
				ImGui::SliderFloat("Radius", &ar1, 0, 10);
				ImGui::SliderFloat("Sectors", &ar2, 0, 72);
				ImGui::SliderFloat("Stacks", &ar3, 0, 36);
				if (ImGui::MenuItem("Create!"))
				{
					if (ar1 * ar2 * ar3 != 0)
						CreateBasicForm(PrimitiveTypes::Primitive_Sphere, ar1, ar2, ar3);
					else
						LOG("[error]Tried to create prtimitive mesh, but some invalid argument was zero!");
				}
				ImGui::EndMenu();
			}
			//Cylinders
			if (ImGui::BeginMenu("Cylinders")) {
				ImGui::SliderFloat("Base Radius", &ar1, 0, 10);
				ImGui::SliderFloat("Top Radius", &ar2, 0, 10);
				ImGui::SliderFloat("Height", &ar3, 0, 10);
				ImGui::SliderFloat("Sectors", &ar4, 0, 72);
				ImGui::SliderFloat("Stacks", &ar5, 0, 36);
				if (ImGui::MenuItem("Create!"))
				{
					if (ar1 * ar2 * ar3 * ar4 * ar5 != 0)
						CreateBasicForm(PrimitiveTypes::Primitive_Cylinder, ar1, ar2, ar3, ar4, ar5);
					else
						LOG("[error]Tried to create prtimitive mesh, but some invalid argument was zero!");
				}
				ImGui::EndMenu();
			}
			//Box
			if (ImGui::BeginMenu("Box")) {
				ImGui::Text("Box param:");
				ImGui::SliderFloat("Base Width", &ar1, 0, 10);
				ImGui::SliderFloat("Base Height", &ar2, 0, 10);
				ImGui::SliderFloat("Box Height", &ar3, 0, 10);
				if (ImGui::MenuItem("Create!"))
				{
					if (ar1 * ar2 * ar3 != 0)
						CreateBasicForm(PrimitiveTypes::Primitive_Box, ar1, ar2, ar3);
					else
						LOG("[error]Tried to create prtimitive mesh, but some invalid argument was zero!");
				}
				ImGui::EndMenu();
			}
			//Cube
			if (ImGui::BeginMenu("Cube")) {
				ImGui::Text("Cube param:");
				ImGui::SliderFloat("Size", &ar1, 0, 10);
				if (ImGui::MenuItem("Create!"))
				{
					if (ar1 != 0)
						CreateBasicForm(PrimitiveTypes::Primitive_Cube, ar1);
					else
						LOG("[error]Tried to create prtimitive mesh, but some invalid argument was zero!");
				}
				ImGui::EndMenu();
			}

			//Cone
			if (ImGui::BeginMenu("Cone")) {
				ImGui::Text("Cone param:");
				ImGui::SliderFloat("Base Radius", &ar1, 0, 10);
				ImGui::SliderFloat("Height", &ar2, 0, 10);
				ImGui::SliderFloat("Sectors", &ar3, 0, 72);
				ImGui::SliderFloat("Stacks", &ar4, 0, 36);
				if (ImGui::MenuItem("Create!"))
				{
					if (ar1 * ar2 * ar3 * ar4 != 0)
						CreateBasicForm(PrimitiveTypes::Primitive_Cone, ar1, ar2, ar3, ar4);
					else
						LOG("[error]Tried to create prtimitive mesh, but some invalid argument was zero!");
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows", true))
		{
			ImGui::MenuItem("3D Viewport", NULL, &show3DWindow);
			ImGui::MenuItem("Console", NULL, &showConsoleWindow);
			ImGui::MenuItem("Hierarchy", NULL, &showHierarchy);
			ImGui::MenuItem("Inspector", NULL, &showInspector);
			ImGui::MenuItem("Active Resurces", NULL, &showResourcesActive);
			ImGui::MenuItem("Example Window", NULL, &showDemoWindow);
			ImGui::MenuItem("Assets Explorer", NULL, &showLoadFileWindow);
			ImGui::EndMenu();
		}
		//if (ImGui::Button("TEST BUTTON", ImVec2(125, 20)))
		//{
		//	LOG("Testing save scene:");
		//	char*  file;
		//	int size = Importer::SerializeScene(App->scene->root, &file );			//DONT DELETE THIS!
		//	App->fileSystem->SavePHO("TestingSaveScene.pho",file,size);
		//	Importer::LoadScene(file, App->scene->root);
		//	delete file;
		//	file = nullptr;
		//}

		//testing code for button disable
		GameStateEnum state = App->GetGameState();

		bool isGamePlaying = true;//when this is true, the color of the play button should turn blue, also enable both pause & step
		bool isGamePaused = false;//when this is true the color of the pause button should turn blue

		if (state == GameStateEnum::STOPPED)
			isGamePlaying = false;
		if (state == GameStateEnum::PAUSED || state == GameStateEnum::ADVANCEONE)
			isGamePaused = true;

		//Play button
		if (isGamePlaying)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.5f, 1.0f, 1.0f));
		}
		if (ImGui::ArrowButton("Play##button", ImGuiDir_Right))
		{
			if (isGamePlaying)
			{
				App->SetNewGameState(GameStateEnum::STOPPED);
				LOG("Loading temporal scene");
				delete App->scene->root; App->scene->root = nullptr;
				App->scene->root = new GameObject(nullptr, "SceneRoot", float4x4::identity, false); //born in the ghetto

				std::string auxPath = LIB_PATH; auxPath += "Scenes/TemporalScene.pho";
				char* auxB = "";
				App->fileSystem->Load(auxPath.c_str(), &auxB);
				Importer::LoadScene(auxB, App->scene->root);
				RELEASE_ARRAY(auxB);
				auxB = nullptr;

			}
			else
			{
				char* auxB = "";
				App->SetNewGameState(GameStateEnum::PLAYED);
				LOG("Saving tempooral scene");
				unsigned int size = Importer::SerializeScene(App->scene->root, &auxB);
				std::string auxPath = LIB_PATH; auxPath += "Scenes/TemporalScene.pho";
				App->fileSystem->SavePHO(auxPath.c_str(), auxB, size);
				RELEASE_ARRAY(auxB);
				auxB = nullptr;
			}
		}
		if (isGamePlaying)
		{
			ImGui::PopStyleColor();
		}

		//Pause button
		if (!isGamePlaying)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		else if (isGamePaused)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.5f, 1.0f, 1.0f));
		}
		if (ImGui::Button("Pause##button"))
		{
			if (isGamePaused)
			{
				App->SetNewGameState(GameStateEnum::PLAYED);
			}
			else
			{
				App->SetNewGameState(GameStateEnum::PAUSED);
			}
		}
		if (!isGamePlaying)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		else if (isGamePaused)
		{
			ImGui::PopStyleColor();
		}

		//Step button
		if (!isGamePlaying)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button("Step Frame##button"))
		{
			App->SetNewGameState(GameStateEnum::ADVANCEONE);
		}
		if (!isGamePlaying)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		//end of testing code
	}
	ImGui::EndMainMenuBar();

	if (showHierarchy)
	{
		if (ImGui::Begin("Hierarchy", &showHierarchy))
		{
			ShowHierarchyTab();

			ImGui::BeginChild("HierarchyChild");
			ImGui::EndChild();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("OBJ_ID"))
				{
					IM_ASSERT(payload->DataSize == sizeof(unsigned int));

					GameObject* newObj = nullptr;
					App->scene->root->GetChildWithID(*(const unsigned int*)payload->Data, newObj);
					newObj->ChangeParent(nullptr);
				}
				ImGui::EndDragDropTarget();

			}




		}
		ImGui::End();
	}
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
	if (showInspector)
	{
		if (ImGui::Begin("Inspector", &showInspector))
		{
			App->scene->UpdateInfoOnSelectedGameObject();

		}
		ImGui::End();
	}
	if (showAboutWindowbool)
	{
		showAboutWindow();
	}
	if (showConfig)
	{
		showConfigFunc();
	}
	if (showConsoleWindow)
	{
		ShowExampleAppConsole(&showConsoleWindow);
	}
	if (show3DWindow)
	{
		Show3DWindow();
	}
	if (showResourcesActive)
	{
		ShowResourcesActive();
	}
	if (showLoadScenes)
	{
		if (ImGui::Begin("Scene Explorer", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild("Scene Browser", ImVec2(0, 300), true);

			DrawDirectoryTreeLoadScene("Library/Scenes");

			ImGui::EndChild();
			ImGui::PopStyleVar();


			ImGui::PushItemWidth(250.f);
			ImGui::InputText("##scene_selector", selectedScene, 250, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);


			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (ImGui::Button("OK", ImVec2(50, 20)))
			{
				LOG("Loading absolute scene");
				delete App->scene->root; App->scene->root = nullptr;
				App->scene->root = new GameObject(nullptr, "SceneRoot", float4x4::identity, false); //born in the ghetto

				char* aux = "";
				std::string selected = selectedScene;
				int size = App->fileSystem->Load(selected.c_str(), &aux);

				if (size != 0)
				{
					Importer::LoadScene(aux, App->scene->root);
					RELEASE_ARRAY(aux);
					aux = nullptr;
				}
				selectedScene[0] = '\0';
				showLoadScenes = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Back", ImVec2(50, 20)))
			{
				showLoadScenes = false;
				selectedScene[0] = '\0';
			}
		}
		ImGui::End();
	}

	if (showLoadFileWindow)
	{
		if (ImGui::Begin("Asset Explorer##window", &showLoadFileWindow))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild("File Browser##window", ImVec2(0, 300), true);

			DrawDirectoryTree("Assets");

			ImGui::EndChild();
			ImGui::PopStyleVar();

			ImGui::PushItemWidth(250.f);
			ImGui::InputText("##file_selector", selectedFile, 250, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);


			ImGui::PopItemWidth();

			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Button("Load##fileExplorer", ImVec2(50, 20)))
			{
				//TODO Depending on what you want to load (ex. PNG), load it into the selected object
				std::string selected = selectedFile;
				if (selected != "")
				{

					Resource* r = App->rManager->ManageAssetUpdate(selected.c_str());
					if (r != nullptr)
					{
						App->rManager->TryLoadResIntoScene(r->GetUID());
					}
				}
				else
				{
					LOG("[warning]We are sorry but we are afraid we may not be able to load a blank space into memory");
				}

				selectedFile[0] = '\0';
			}

			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::SameLine();
			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Button("Cancel##fileExplorer", ImVec2(50, 20)))
			{
				selectedFile[0] = '\0';
			}
			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::SameLine();
			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 1.0f, 0.5f, 0.25f));
			if (ImGui::Button("Refresh##fileExplorer", ImVec2(60, 20)))
			{
				App->rManager->haveToReload = true;
			}
			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}


			ImGui::SameLine();
			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.25f, 0.0f, 1.0f));
			if (ImGui::Button("Delete##fileExplorer", ImVec2(55, 20)))
			{
				ImGui::OpenPopup("Delete Asset?");
			}
			if (App->GetGameState() != GameStateEnum::STOPPED)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			int flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding;
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Delete Asset?", NULL, flags))
			{
				ImGui::Text("This will deleta asset and meta from Asset folders and lib \n");
				ImGui::Text("It will also remove itself from loaded memory. Are you sure ? \n");
				ImGui::Separator();

				if (ImGui::Button("Delete", ImVec2(120, 0)))
				{
					App->fileSystem->DeleteFromAssetsAndLibs(selectedFile);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0)) || (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor(2);
		}
		ImGui::End();
	}





	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP)
		showQuit = true;

	if (showQuitPopup())
		status = UPDATE_STOP;


	return status;
}

update_status ModuleGUI::PostUpdate(float dt)
{
	update_status status = UPDATE_CONTINUE;

	return status;
}

bool ModuleGUI::CleanUp()
{
	bool ret = true;

	
	//Void functions, no return, no check possible.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	selectedScene[0] = '\0';


	return ret;
}

void ModuleGUI::OnResize(int width, int height)
{

	//ImVec2 windowSize = ImVec2(width, height);

	imgSize = ImVec2(width, height); //Not that window but the one we are printing in

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//App->renderer3D->ProjectionMatrix = perspective(App->camera->foV, (float)width / (float)height, App->camera->nearPlaneDist, App->camera->farPlaneDist);
	//glLoadMatrixf(&App->renderer3D->ProjectionMatrix);
	App->camera->editorCam->SetNewAspectRatio(width, height);
	glLoadMatrixf(&App->camera->editorCam->GetProjMat().v[0][0]);


	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&App->camera->editorCam->GetViewMat().v[0][0]);

	//App->renderer3D->OnResize(width, height);

}

void ModuleGUI::Draw()
{
	// Rendering
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui::Render(); ImGuiIO& io = ImGui::GetIO(); (void)io;
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

bool ModuleGUI::showAboutWindow()
{
	//this can't be externalized because of ShellExecuteA command + SDL references

	if (!ImGui::Begin("About this Engine", &showAboutWindowbool))		//this is how you add the cross button to a window
	{
		ImGui::End();
		return false;
	}


	//name of engine
	const char* aux = "NewClockWorkEngine";
	ImGui::Spacing();
	ImGui::Indent();
	ImGui::TextColored(ImVec4(153, 153, 000, 250), aux); //(0, 100, 130, 150)
	ImVec2 vecaux = ImGui::GetCursorPos();
	ImGui::Unindent();
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Description"))
	{
		ImGui::Indent();
		//descrption
		ImGui::Text("Welcome to NewClockWorkEngine! This engine is made by two 3rd grade students of Design and Development of Videogames on CITM university");
		ImGui::Text("The intent is to create a 3D functional engine with a focus on sound design, performance and capability (hence the engine name)");
		ImGui::Text("We hope you'll enjoy using it as much as we enjoyed making it!");
		ImGui::Spacing();
		(ImGui::Text("Check out the GitHub:")); ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		if (ImGui::Button("Here!"))
		{
			ShellExecuteA(NULL, NULL, "https://github.com/xsiro/NewClockWorkEngine", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::Unindent();
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Authors"))
	{
		ImGui::Indent();
		//autohors
		ImGui::Text("This Engine was made by Alex Lopez Agudo, Daniel Ruiz Cano & Victor Nisa Sanchez");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Check us out in Github:"); ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		if (ImGui::Button("Here!"))
		{
			ShellExecuteA(NULL, NULL, "https://github.com/alexla99", NULL, NULL, SW_SHOWNORMAL);
			ShellExecuteA(NULL, NULL, "https://github.com/victornisa", NULL, NULL, SW_SHOWNORMAL);
			ShellExecuteA(NULL, NULL, "https://github.com/xsiro", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::Unindent();
	}

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Licensing"))
	{
		ImGui::Indent();
		if (ImGui::TreeNode("Libraries Used"))
		{
			ImGui::Indent();

			ImGui::Text("MathGeoLib v1.5"); //TODO: MathGeoLib has no get version lel, stuck on 1.5 since 2004 or something
			ImGui::Spacing();

			ImGui::Text("ImGui");
			ImGui::SameLine();
			ImGui::Text(ImGui::GetVersion());
			ImGui::Spacing();

			ImGui::Text("SDL");
			ImGui::SameLine();
			SDL_version compiled;
			SDL_VERSION(&compiled);
			ImGui::Text("%d.%d.%d", compiled.major, compiled.minor, compiled.patch); //Because returning a string was too easy huh
			ImGui::Spacing();

			ImGui::Text("OpenGL v.");
			GLint aux1; (glGetIntegerv(GL_MAJOR_VERSION, &aux1));
			GLint aux2; (glGetIntegerv(GL_MINOR_VERSION, &aux2));
			ImGui::SameLine();
			ImGui::Text("%i.", int(aux1));
			ImGui::SameLine();
			ImGui::Text("%i.", int(aux2));
			ImGui::Spacing();


			ImGui::Text("Devil v.");
			int devilVer = ilutGetInteger(ILUT_VERSION_NUM);
			ImGui::SameLine();
			ImGui::Text("%i.", int(devilVer));
			ImGui::Spacing();

			ImGui::Unindent();
			ImGui::TreePop();
		}
		ImGui::Text("Check the engine license:"); ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		if (ImGui::Button("Here!"))
		{
			ShellExecuteA(NULL, NULL, "https://opensource.org/licenses/MIT", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::Unindent();
	}

	ImGui::End();

	return true;
}


bool ModuleGUI::showConfigFunc()
{
	if (!ImGui::Begin("Configuration", &showConfig))		//this is how you add the cross button to a window
	{
		ImGui::End();
		return false;
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Set window background to black


	if (ImGui::CollapsingHeader("FPS")) {

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set window background to white
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.29f, 0.48f, 0.54f));
		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", App->fpsBuffer[App->fpsBuffer.size() - 1]);
		ImGui::PlotHistogram("##framerate", &App->fpsBuffer[0], App->fpsBuffer.size(), 0, title, 0.0f, 120.0f, ImVec2(400, 100));


		sprintf_s(title, 25, "Milliseconds %.1f", App->millisecondsBuffer[App->millisecondsBuffer.size() - 1]);
		ImGui::PlotHistogram("##ms", &App->millisecondsBuffer[0], App->millisecondsBuffer.size(), 0, title, 0.0f, 100.0f, ImVec2(400, 100));
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("FrameCount: %u", App->GetFrameCount());
		ImGui::Text("GameClock:  %.3f", App->GetTime());
		ImGui::Text("Game DT:    %.3f", App->GetGameDT());
		ImGui::Text("TimeScale:  %.3f", App->GetTimeScale());
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("RealClock:  %.3f", App->GetRealTime());
		ImGui::Text("Engine DT:  %.3f", App->GetRealDT());



		ImGui::PopStyleColor(2);

	}

	if (ImGui::CollapsingHeader("Window")) {

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set window background to white

		if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0, 1))
		{
			SDL_SetWindowBrightness(App->window->window, App->window->brightness);

		}


		if (ImGui::SliderInt("Width", &(App->window->w), 100, 2800))
		{
			if (resizable) {
				App->window->ResizeWindow(App->window->w, App->window->h);
				//SDL_SetWindowSize(App->window->window, App->window->w, App->window->h);
				//OpenGLOnResize(App->window->w, App->window->h);
				App->renderer3D->OnResize(App->window->w, App->window->h);
			}
		};
		if (ImGui::SliderInt("Height", &(App->window->h), 1, 2800))
		{
			if (resizable) {
				App->window->ResizeWindow(App->window->w, App->window->h);
				//SDL_SetWindowSize(App->window->window, App->window->w, App->window->h);
				//OpenGLOnResize(App->window->w, App->window->h);
				App->renderer3D->OnResize(App->window->w, App->window->h);
			}
		};
		if (ImGui::Checkbox("Resizable", &resizable))
		{
			if (resizable) {
				LOG("Resizable mode toggled to TRUE");
			}
			else {
				LOG("Resizable mode toggled to FALSE");
			}
		}
		if (ImGui::Checkbox("Vsync", &Vsync))
		{
			SDL_GL_SetSwapInterval(Vsync);
		}
		ImGui::PopStyleColor();

	}
	if (ImGui::CollapsingHeader("Hardware specs")) {

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set window background to white

		int lineSize = SDL_GetCPUCacheLineSize();
		ImGui::Text("GPU has size %i", lineSize);
		int GPUCount = SDL_GetCPUCount();
		ImGui::Text("PC has %i CPU's", GPUCount);
		int systemRAM = SDL_GetSystemRAM();
		ImGui::Text("Ram configured is %i", systemRAM);
		ImGui::Text("Caps:");
		ImGui::SameLine(); if (SDL_Has3DNow()) { ImGui::Text("3DNow "); }
		ImGui::SameLine(); if (SDL_HasAVX()) { ImGui::Text("AVX "); }
		ImGui::SameLine(); if (SDL_HasAltiVec()) { ImGui::Text("AltiVec "); }
		ImGui::SameLine(); if (SDL_HasMMX()) { ImGui::Text("MMX "); }
		ImGui::SameLine(); if (SDL_HasRDTSC()) { ImGui::Text("RDTSC "); }
		ImGui::SameLine(); if (SDL_HasSSE()) { ImGui::Text("SSE "); }
		ImGui::SameLine(); if (SDL_HasSSE2()) { ImGui::Text("SSE2 "); }
		ImGui::SameLine(); if (SDL_HasSSE3()) { ImGui::Text("SSE3 "); }
		ImGui::SameLine(); if (SDL_HasSSE41()) { ImGui::Text("SSE41 "); }
		ImGui::SameLine(); if (SDL_HasSSE42()) { ImGui::Text("SSE42 "); }

		GLint tmp_GPUTotalVRAM = 0;
		glGetIntegerv(0x9048, &tmp_GPUTotalVRAM);
		tmp_GPUTotalVRAM /= KBTOMB;
		ImGui::Text("Total VRAM is %i", tmp_GPUTotalVRAM);

		GLint tmp_GPUCurrentVRAM = 0;
		glGetIntegerv(0x9049, &tmp_GPUCurrentVRAM);
		tmp_GPUCurrentVRAM /= KBTOMB;
		ImGui::Text("Current VRAM is %i", tmp_GPUCurrentVRAM);

		ImGui::PopStyleColor();
	}

	if (ImGui::CollapsingHeader("Rendering"))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set window background to white

		if (ImGui::DragFloat("Gizmo Size", &gizmoSize, 0.01f, 0.1f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
		{
			gizmoSize = max(gizmoSize, 0.1f);
			gizmoSize = min(gizmoSize, 1.0f);
		}

		if (ImGui::DragFloat("Outline Scale", &App->renderer3D->outlineScale, 0.01f, 1.0f, 1.5f, "%.3f", ImGuiSliderFlags_Logarithmic))
		{
			App->renderer3D->outlineScale = max(App->renderer3D->outlineScale, 1.0f);
			App->renderer3D->outlineScale = min(App->renderer3D->outlineScale, 1.5f);
		}

		ImGui::Checkbox("Draw Grid", &App->renderer3D->drawGrid);

		ImGui::Checkbox("Draw DebugCamRay", &App->renderer3D->drawDebugRay);

		if (ImGui::DragFloat("Grid Lenght", &App->renderer3D->gridLength, 0.1f, 1.0f, 1000.0f, "%.3f"))
		{
			//
		}


		if (ImGui::Checkbox("Depth Testing", &App->renderer3D->depthTesting))
		{
			if (App->renderer3D->depthTesting)glEnable(GL_DEPTH_TEST);
			else glDisable(GL_DEPTH_TEST);
		}

		if (ImGui::Checkbox("Depth Display", &App->renderer3D->showDepth))
		{
			App->renderer3D->GenerateBuffers(App->window->w, App->window->h);
		}

		if (ImGui::Checkbox("Cull Faces", &App->renderer3D->cullFace))
		{
			if (App->renderer3D->cullFace)glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
		}


		if (ImGui::Checkbox("Lighting", &App->renderer3D->lighting))
		{
			if (App->renderer3D->lighting)glEnable(GL_LIGHTING);
			else glDisable(GL_LIGHTING);
		}


		if (ImGui::Checkbox("Color Material", &App->renderer3D->colorMaterial))
		{
			if (App->renderer3D->colorMaterial)glEnable(GL_COLOR_MATERIAL);
			else glDisable(GL_COLOR_MATERIAL);
		}


		if (ImGui::Checkbox("Texture 2D", &App->renderer3D->texture2D))
		{
			if (App->renderer3D->texture2D)glEnable(GL_TEXTURE_2D);
			else glDisable(GL_TEXTURE_2D);
		}

		//TODO group combos into a function
		const char* drawModes[] = { "BOTH","FILL","WIREFRAME" };
		const char* drawLabel = drawModes[(int)App->scene->maxSceneDrawMode];  // Label to preview before opening the combo (technically it could be anything)
		if (ImGui::BeginCombo("Scene draw Mode", drawLabel, ImGuiComboFlags_PopupAlignLeft))
		{
			for (int n = 0; n < IM_ARRAYSIZE(drawModes); n++)
			{
				const bool is_selected = ((int)App->scene->maxSceneDrawMode == n);
				if (ImGui::Selectable(drawModes[n], is_selected))
					App->scene->maxSceneDrawMode = (MeshDrawMode)n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}



		ImGui::Checkbox("Display all AABBs", &App->renderer3D->displayAABBs);


		ImGui::PopStyleColor();
	}

	if (ImGui::CollapsingHeader("camera"))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set window background to white

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::DragFloat("Camera speed", &App->camera->camSpeed, 0.1f, 0.01f, FLT_MAX, "%.3f", 0);
		ImGui::DragFloat("Camera speed multiplier", &App->camera->camSpeedMult, 1.0f, 0.01f, FLT_MAX, "%.3f", 0);

		ImGui::Spacing();
		ImGui::Spacing();
		float camFoV = App->camera->editorCam->GetFoV();
		if (ImGui::SliderFloat("FoV", &camFoV, 1.0f, 90.0f))
		{
			App->camera->editorCam->SetNewFoV(camFoV);
			OnResize(App->window->w, App->window->h);
		};

		ImGuiSliderFlags flags = ImGuiSliderFlags_None;
		int sensitivity = 100;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)sensitivity = 1000000; //use Lshift to be more precise

		float nearPDist = App->camera->editorCam->GetNearPlaneDist();
		float farPDist = App->camera->editorCam->GetFarPlaneDist();
		if (ImGui::DragFloat("Near Plane", &nearPDist, farPDist / sensitivity, 0.01f, farPDist, "%.3f", flags))
		{
			App->camera->editorCam->SetNearPlane(nearPDist);
			OnResize(App->window->w, App->window->h);
		}

		if (ImGui::DragFloat("Far Plane", &farPDist, (abs(farPDist - nearPDist)) / sensitivity, nearPDist, 2000.0f, "%.3f", flags))
		{
			App->camera->editorCam->SetFarPlane(farPDist);
			OnResize(App->window->w, App->window->h);
		}


		ImGui::PopStyleColor();
	}

	ImGui::PopStyleColor();
	ImGui::End();
	return true;
}

bool ModuleGUI::showQuitPopup()
{
	bool ret = false;
	bool popupNew = false;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7.0f, 7.0f));
	ImGui::PushStyleColor(ImGuiCol_ModalWindowDarkening, { 0.05f,0.05f,0.1f,0.75f });
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.6f, 0.0f, 0.8f));

	if (showQuit && !quitAlreadyOpened)
	{
		ImGui::OpenPopup("Quit?");
		showQuit = false;
		quitAlreadyOpened = true;
		popupNew = true;
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	int flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding;

	if (ImGui::BeginPopupModal("Quit?", NULL, flags))
	{
		ImGui::Text("All your progress will be lost!\nAre you sure you want to quit?\n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ret = true; ImGui::CloseCurrentPopup(); quitAlreadyOpened = false; showQuit = false; }
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)) || (!popupNew && App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP))
		{
			ImGui::CloseCurrentPopup();
			quitAlreadyOpened = false;
			showQuit = false;
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();
	return ret;
}

bool ModuleGUI::ShowResourcesActive()
{

	ActiveResources act;

	if (ImGui::Begin("Active Resources", &showResourcesActive))		//this is how you add the cross button to a window
	{
		ImGui::Checkbox("ShowOnlyLoaded##resources", &showOnlyLoadedRes);
		//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (showOnlyLoadedRes)
		{
			act = App->rManager->GetActiveResources();
		}
		else
		{
			act = App->rManager->GetActiveResources(true);
		}

		//std::sort(act.meshes.begin(), act.meshes.end());
		//std::sort(act.models.begin(), act.models.end());
		//std::sort(act.textures.begin(), act.textures.end());
		//std::sort(act.scenes.begin(), act.scenes.end());

		std::string selectableString = "";

		if (ImGui::CollapsingHeader("Textures##resources"))
		{
			//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			for (int i = 0; i < act.textures.size(); i++)
			{
				if (!act.textures[i]->IsLoadedInMemory())
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

				selectableString = act.textures[i]->GetName() + "##" + std::to_string(act.textures[i]->GetUID());
				ImGui::Selectable(selectableString.c_str());

				//DragDropSource Begin
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					// Set payload to carry the object pointer
					unsigned int payloadID = act.textures[i]->GetUID();
					ImGui::SetDragDropPayload("ResourceTex##dragdropSource", &payloadID, sizeof(unsigned int));

					// Display preview (could be anything, e.g. when dragging an image we could decide to display
					// the filename and a small preview of the image, etc.)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4 FOCUSED_COLOR);
					ImGui::Text("%s", act.textures[i]->GetName().c_str());
					ImGui::PopStyleColor();

					ImGui::EndDragDropSource();
				}
				//DragDropSource End

				if (ImGui::IsItemHovered())//TODO the tooltip is the same for every case, consider creating a method
				{
					ImGui::BeginTooltip();

					std::string count = std::to_string(act.textures[i]->referenceCount);
					std::string ID = std::to_string(act.textures[i]->GetUID());
					std::string assetPath = act.textures[i]->GetAssetFile();
					std::string libPath = act.textures[i]->GetLibraryFile();

					ImGui::Text("ID: %s", ID.c_str());
					ImGui::Text("References: %s", count.c_str());
					ImGui::Text("Asset Path: %s", assetPath.c_str());
					ImGui::Text("Lib Path: %s", libPath.c_str());


					ImGui::EndTooltip();
				}
				if (!act.textures[i]->IsLoadedInMemory())
					ImGui::PopStyleColor();
			}
			//ImGui::PopStyleColor();
		}
		if (ImGui::CollapsingHeader("Meshes##resourcesm"))
		{
			//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			for (int i = 0; i < act.meshes.size(); i++)
			{
				if (!act.meshes[i]->IsLoadedInMemory())
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

				selectableString = act.meshes[i]->GetName() + "##" + std::to_string(act.meshes[i]->GetUID());
				ImGui::Selectable(selectableString.c_str());

				//DragDropSource Begin
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					// Set payload to carry the object pointer
					unsigned int payloadID = act.meshes[i]->GetUID();
					ImGui::SetDragDropPayload("ResourceMesh##dragdropSource", &payloadID, sizeof(unsigned int));

					// Display preview (could be anything, e.g. when dragging an image we could decide to display
					// the filename and a small preview of the image, etc.)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4 FOCUSED_COLOR);
					ImGui::Text("%s", act.meshes[i]->GetName().c_str());
					ImGui::PopStyleColor();

					ImGui::EndDragDropSource();
				}
				//DragDropSource End

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					std::string count = std::to_string(act.meshes[i]->referenceCount);
					std::string ID = std::to_string(act.meshes[i]->GetUID());
					std::string assetPath = act.meshes[i]->GetAssetFile();
					std::string libPath = act.meshes[i]->GetLibraryFile();

					ImGui::Text("ID: %s", ID.c_str());
					ImGui::Text("References: %s", count.c_str());
					ImGui::Text("Asset Path: %s", assetPath.c_str());
					ImGui::Text("Lib Path: %s", libPath.c_str());

					ImGui::EndTooltip();
				}
				if (!act.meshes[i]->IsLoadedInMemory())
					ImGui::PopStyleColor();
			}
			//ImGui::PopStyleColor();
		}
		if (ImGui::CollapsingHeader("Scenes##resources"))
		{
			//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			for (int i = 0; i < act.scenes.size(); i++)
			{
				if (!act.scenes[i]->IsLoadedInMemory())
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

				selectableString = act.scenes[i]->GetName() + "##" + std::to_string(act.scenes[i]->GetUID());
				ImGui::Selectable(selectableString.c_str());
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					std::string count = std::to_string(act.scenes[i]->referenceCount);
					std::string ID = std::to_string(act.scenes[i]->GetUID());
					std::string assetPath = act.scenes[i]->GetAssetFile();
					std::string libPath = act.scenes[i]->GetLibraryFile();

					ImGui::Text("ID: %s", ID.c_str());
					ImGui::Text("References: %s", count.c_str());
					ImGui::Text("Asset Path: %s", assetPath.c_str());
					ImGui::Text("Lib Path: %s", libPath.c_str());

					ImGui::EndTooltip();
				}
				if (!act.scenes[i]->IsLoadedInMemory())
					ImGui::PopStyleColor();
			}
			//ImGui::PopStyleColor();
		}
		if (ImGui::CollapsingHeader("Models##resources"))
		{
			//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			for (int i = 0; i < act.models.size(); i++)
			{
				if (!act.models[i]->IsLoadedInMemory())
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

				selectableString = act.models[i]->GetName() + "##" + std::to_string(act.models[i]->GetUID());
				ImGui::Selectable(selectableString.c_str());
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					std::string count = std::to_string(act.models[i]->referenceCount);
					std::string ID = std::to_string(act.models[i]->GetUID());
					std::string assetPath = act.models[i]->GetAssetFile();
					std::string libPath = act.models[i]->GetLibraryFile();

					ImGui::Text("ID: %s", ID.c_str());
					ImGui::Text("References: %s", count.c_str());
					ImGui::Text("Asset Path: %s", assetPath.c_str());
					ImGui::Text("Lib Path: %s", libPath.c_str());

					ImGui::EndTooltip();
				}
				if (!act.models[i]->IsLoadedInMemory())
					ImGui::PopStyleColor();
			}
			//ImGui::PopStyleColor();
		}

		//ImGui::PopStyleColor();
		ImGui::End();
	}
	return true;
}

void ModuleGUI::DrawDirectoryTree(const char* newDir)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((newDir) ? newDir : "");
	dir += "/";

	App->fileSystem->GetDirFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str()))
		{
			DrawDirectoryTree((dir + (*it)).c_str());
			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());//sorts all elements in the string list

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;
		//we check if its a ".meta" file and if so we do not display them
		std::string extension;
		unsigned int index = str.find_last_of(".");

		if (index < str.size())
		{
			extension = str.substr(index); //look for the last instance of a point. Format should be next
		}

		if (extension != ".meta")
		{
			//if it has no meta don't bother drawing it
			std::string directory = newDir;
			std::string metapath = directory + "/" + str + ".meta";
			if (App->fileSystem->DoesFileExist(metapath.c_str()))
			{

				if (ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemClicked()) {
						sprintf_s(selectedFile, 250, "%s%s", dir.c_str(), str.c_str());
					}
				}

				ImGui::TreePop();
			}
		}
	}
}

void ModuleGUI::DrawDirectoryTreeLoadScene(const char* newDir)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((newDir) ? newDir : "");
	dir += "/";

	App->fileSystem->GetDirFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str()))
		{
			DrawDirectoryTreeLoadScene((dir + (*it)).c_str());
			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());//sorts all elements in the string list

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;
		//we check if its a ".meta" file and if so we do not display them
		std::string extension;
		unsigned int index = str.find_last_of(".");

		if (index < str.size())
		{
			extension = str.substr(index); //look for the last instance of a point. Format should be next
		}

		if (extension != ".meta" && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::IsItemClicked()) {
				sprintf_s(selectedScene, 250, "%s%s", dir.c_str(), str.c_str());
			}

			ImGui::TreePop();
		}
	}
}

bool ModuleGUI::CreateBasicForm(PrimitiveTypes type, float ar1, float ar2, float ar3, float ar4, float ar5)
{
	//We would love to make a switch, but we cant. Happyness huh
	bool ret = false;


	if (type == PrimitiveTypes::Primitive_Cube)
		//ar1 = size;
	{
		std::vector<float> vertexArrayCube = {
	0.f, 0.f, 0.f,
	ar1, 0.f, 0.f,
	ar1, 0.f, -ar1,
	0, 0, -ar1,

	0.f, ar1, 0.f,
	ar1, ar1, 0.f,
	ar1, ar1, -ar1,
	0.f, ar1, -ar1
		};
		std::vector<unsigned int> indexArrayCube = {
			4, 0, 1,
			1, 5, 4,

			4, 7, 3,
			3, 0, 4,

			2, 3, 7,
			7, 6, 2,

			7, 4, 5,
			5, 6, 7,

			5, 1, 2,
			2, 6, 5,

			0, 3, 2,
			2, 1, 0
		};
		CreateMeshfromPrimAndSendToScene(vertexArrayCube, indexArrayCube, "Cube");
		ret = true;
	}
	if (type == PrimitiveTypes::Primitive_Sphere)
	{
		std::vector<float> vertexArray;
		std::vector<unsigned int> indexArray;
		SphereFillVectorsVertexAndIndex(vertexArray, indexArray, ar1, (int)ar2, (int)ar3);
		CreateMeshfromPrimAndSendToScene(vertexArray, indexArray, "Sphere");
		ret = true;
	}
	if (type == PrimitiveTypes::Primitive_Cylinder)
	{
		std::vector<float> vertexArray;
		std::vector<unsigned int> indexArray;
		CylinderFillVectorsVertexAndIndex(vertexArray, indexArray, ar1, ar2, ar3, (int)ar4, (int)ar5);
		CreateMeshfromPrimAndSendToScene(vertexArray, indexArray, "Cylinder");
		ret = true;
	}
	if (type == PrimitiveTypes::Primitive_Cone)
	{
		std::vector<float> vertexArray;
		std::vector<unsigned int> indexArray;
		ConeFillVectorsVertexAndIndex(vertexArray, indexArray, ar1, ar2, (int)ar3, (int)ar4);
		CreateMeshfromPrimAndSendToScene(vertexArray, indexArray, "Cone");
		ret = true;
	}
	if (type == PrimitiveTypes::Primitive_Box)
	{
		std::vector<float> vertexArrayCube = {
			0.f, 0.f, 0.f,
			ar1, 0.f, 0.f,
			ar1, 0.f, -ar3,
			0, 0, -ar3,

			0.f, ar2, 0.f,
			ar1, ar2, 0.f,
			ar1, ar2, -ar3,
			0.f, ar2, -ar3
		};
		std::vector<unsigned int> indexArrayCube = {
			4, 0, 1,
			1, 5, 4,

			4, 7, 3,
			3, 0, 4,

			2, 3, 7,
			7, 6, 2,

			7, 4, 5,
			5, 6, 7,

			5, 1, 2,
			2, 6, 5,

			0, 3, 2,
			2, 1, 0
		};
		CreateMeshfromPrimAndSendToScene(vertexArrayCube, indexArrayCube, "Box");
		ret = true;
	}

	return ret;
}

void ModuleGUI::CreateMeshfromPrimAndSendToScene(std::vector<float> vertices, std::vector<unsigned int> indices, std::string name)
{
	//TODO this doesn't work for the moment due to a lack of mesh geenration from code, fix it


	//std::vector<float> fakeNormals;
	//for (int i = 0; i < vertices.size(); i++) { //openGL works in misterous ways
	//	fakeNormals.push_back(0);
	//}
	//std::vector<float> fakeTex;
	//for (int i = 0; i < indices.size(); i++) {
	//	fakeTex.push_back(0);
	//	fakeTex.push_back(0);
	//}
	//ResourceMesh newMesh = ResourceMesh(vertices, indices, fakeNormals, fakeTex, 0);//TODO for the moment we pass id 0 to the mesh
	////AuxM.drawMode = MeshDrawMode::DRAW_MODE_BOTH;
	////App->scene->meshes.push_back(AuxM);
	//std::string newName = "Primitive";
	//if (name != "")
	//	newName = name;

	//GameObject* newObj = new GameObject(App->scene->root, newName, float4x4::identity);
	//newObj->CreateComponent(ComponentType::MESH);
	//newObj->GetComponent<ModuleMesh>()->SetMesh(newMesh);
}

//Deprecated function
void ModuleGUI::OpenGLOnResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glScissor(0, 0, w, h);

	//We set the new screen height & width
	SDL_GetWindowSize(App->window->window, &App->window->w, &App->window->h);

	//Calculate OpenGl stuff-----------------
	float width = (float)App->window->w;
	float height = (float)App->window->h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//App->renderer3D->ProjectionMatrix = perspective(App->camera->foV, (width / 2) / height, App->camera->nearPlaneDist, App->camera->farPlaneDist);
	//glLoadMatrixf(&App->renderer3D->ProjectionMatrix);

	App->camera->editorCam->SetNewAspectRatio(width, height);
	glLoadMatrixf(&App->camera->editorCam->GetProjMat().v[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleGUI::GetViewportRectUI(float2& screenPos, float2& size) const
{
	screenPos.x = imgPos.x;
	screenPos.y = imgPos.y;
	size.x = imgSize.x;
	size.y = imgSize.y;
}

void ModuleGUI::ShowExampleAppConsole(bool* p_open)
{
	console->Draw("Example: Console", p_open);
}

void ModuleGUI::CreateDockingSpace()
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", NULL, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	else
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("[error]: Docking is not enabled! See Demo > Configuration.");
		ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
		ImGui::SameLine(0.0f, 0.0f);
		if (ImGui::SmallButton("click here"))
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	ImGui::End();
}

bool ModuleGUI::Show3DWindow()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	ImGuiStyle& style = ImGui::GetStyle();

	static ImGuiStyle ref_saved_style = style;
	//style.WindowBorderSize = 0.0f;
	style.WindowPadding = ImVec2(0.0f, 0.0f);

	if (!ImGui::Begin("3D Viewport", &show3DWindow, flags))
	{
		ImGui::End();
		return false;
	}



	ImVec2 winSize = ImGui::GetWindowSize();
	winSize.y -= 19;//taking into account the menu bar
	if (winSize.x != imgSize.x || winSize.y != imgSize.y)
		OnResize(winSize.x, winSize.y);

	ImVec2 lastCursorPos = ImGui::GetCursorPos();
	imgPos = ImGui::GetWindowPos();
	imgPos.x = imgPos.x + lastCursorPos.x;
	imgPos.y = imgPos.y + lastCursorPos.y;


	ImGui::Image((ImTextureID)App->renderer3D->renderTex, imgSize, ImVec2(0, 1), ImVec2(1, 0));

	GuizmoEditTransform();

	ImVec2 winPos = ImGui::GetWindowPos();
	winSize = ImGui::GetWindowSize();
	winSize.y -= 19;
	winPos.y += 19;
	ImVec2 mousePos = ImGui::GetMousePos();

	ImVec2 viewportTextOffset = { 5.0f,5.0f };//offset from the image border
	lastCursorPos.x += viewportTextOffset.x;
	lastCursorPos.y += viewportTextOffset.y;
	ImGui::SetCursorPos(lastCursorPos);
	std::string drawModeString;
	switch (App->scene->maxSceneDrawMode)
	{
	case MeshDrawMode::DRAW_MODE_BOTH:
		drawModeString = "FILL + WIRE";
		break;
	case MeshDrawMode::DRAW_MODE_FILL:
		drawModeString = "FILL";
		break;
	case MeshDrawMode::DRAW_MODE_WIRE:
		drawModeString = "WIRE";
		break;
	}

	ImGui::Text("Scene draw Mode: %s", drawModeString.c_str());
	lastCursorPos = ImGui::GetCursorPos();
	lastCursorPos.x += viewportTextOffset.x;
	ImGui::SetCursorPos(lastCursorPos);

	ImGui::Text("Object count: %i", GameObject::numberOfObjects - 1); //-1 for the scene root





	if (ImGui::IsWindowHovered() &&
		mousePos.x >= winPos.x && mousePos.x <= winPos.x + winSize.x && mousePos.y >= winPos.y && mousePos.y <= winPos.y + winSize.y)//inside window
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{

			float2 relativeMousePos; //gets the mouse pos relative to the bottom left corner normalized between -1 and 1
			//being the bottom left corner -1,-1 and the upper right 1,1
			relativeMousePos.x = ((mousePos.x - winPos.x) / (winSize.x * 0.5f)) - 1.0f;
			relativeMousePos.y = -1.0f * (((mousePos.y - winPos.y) / (winSize.y * 0.5f)) - 1.0f);
			LOG("Viewport screen Mouse Position on last click:");
			LOG("Pos X:%f", relativeMousePos.x);
			LOG("Pos Y:%f", relativeMousePos.y);

			App->camera->lastKnowMousePos = relativeMousePos;
			App->camera->viewportClickRecieved = true;
		}


		App->scene->mouseActive = true;//TODO change variable to be in this module ??
	}
	else if (App->scene->mouseActive &&
		!(App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) &&
		!(App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT) &&
		!(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT))
	{
		App->scene->mouseActive = false;
	}


	ImGui::End();
	style = ref_saved_style;
	return true;
}

void ModuleGUI::GuizmoEditTransform()
{
	//operation type
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE)//TODO we have to check this to not make camera inputs not overlap with gizmo ones, we should change this or camera inputs to not make the same checks
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)//makes gizmo to not show when pressing q (if we want to select the object but see it without the gizmo in front of it)
			mCurrentGizmoOperation = ImGuizmo::BOUNDS;
	}

	if (App->scene->selectedGameObjs.empty())
	{
		return;
	}

	GameObject* currGameObj = App->scene->selectedGameObjs.back();



	float4x4 editMat = currGameObj->GetComponent<ModuleTransform>()->GetGlobalTransform().Transposed();
	float4x4 viewMat = App->camera->editorCam->GetViewMat();

	ImGuizmo::SetDrawlist();

	float2 viewportPos;
	float2 viewportSize;
	GetViewportRectUI(viewportPos, viewportSize);
	float2 halfSize = viewportSize / 2;
	float2 centerPos = float2(viewportPos.x + halfSize.x, viewportPos.y + halfSize.y);
	float scale = 0.25f;//It works when scaled by 0.25f but why????!!!
	ImGuizmo::SetGizmoSizeClipSpace(gizmoSize);//scale of the gizmo
	//ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
	ImGuizmo::SetRect(centerPos.x - (halfSize.x * scale), centerPos.y - halfSize.y * scale, viewportSize.x * scale, viewportSize.y * scale);

	ImGuizmo::Manipulate(viewMat.ptr(), App->camera->editorCam->GetProjMat().Transposed().ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, editMat.ptr());


	if (ImGuizmo::IsUsing())//if the user is interacting with the guizmo update matrix
	{
		App->camera->isGizmoInteracting = true;
		currGameObj->GetComponent<ModuleTransform>()->SetGlobalTransform(editMat.Transposed());
	}

}
