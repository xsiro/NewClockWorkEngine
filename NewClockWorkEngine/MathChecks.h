#pragma once
#include "imgui/imgui.h"
#include "MathGeoLib/include/MathGeoLib.h"


bool TestColSphere(LCG seed) {
	int randomRad1 = seed.Int(0, 5);
	Sphere aux1 = Sphere(float3(0, 0, 0), randomRad1);
	int randomRad2 = seed.Int(0, 5);
	Sphere aux2 = Sphere(float3(3, 3, 3), randomRad2);
	return aux1.Intersects(aux2);
}

bool TestColCyl(LCG seed) {
	int randomRad1 = seed.Int(0, 5);
	Cylinder aux1 = Cylinder(float3(0, 0, 0), float3(0, 0, 0), randomRad1);
	int randomRad2 = seed.Int(0, 5);
	Cylinder aux2 = Cylinder(float3(4, 0, 0), float3(4, 0, 0), randomRad2);
	bool cylCol;
	if (randomRad1 + randomRad2 >= 4) //TODO: cheap AF
		cylCol = true;
	else
		cylCol = false;
	return cylCol;
}

bool TestColTri(LCG seed) {
	int randomRad1 = seed.Int(0, 5);
	Triangle aux1 = Triangle(float3(0, 0, 0), float3(randomRad1, 0, 0), float3(randomRad1 / 2, 2, 0));
	int randomRad2 = seed.Int(0, 5);
	Triangle aux2 = Triangle(float3(3, 0, 0), float3(3 + randomRad2, 0, 0), float3(3 + randomRad2 / 2, 2, 0));
	bool triCol;
	if (randomRad1 + randomRad2 >= 3) //TODO: cheap AF
		triCol = true;
	else
		triCol = false;
	return triCol;
}

bool TestColAABB(LCG seed) {
	int randomRad1 = seed.Int(0, 5); Sphere s1(float3(0, 0, 0), randomRad1);
	int randomRad2 = seed.Int(0, 5); Sphere s2(float3(4, 0, 0), randomRad2);

	AABB aux1(s1); AABB aux2(s2);
	AABB auxSol = aux1.Intersection(aux2);

	bool AABBCol;
	if (auxSol.Volume() != 0) {

		AABBCol = true;
	}
	else {
		AABBCol = false;
	}
	return AABBCol;
}

void ShowSphereWindow(bool &showSphereWindow, LCG seed, bool sphereCol) {

	ImGui::Begin("Sphere checks", &showSphereWindow);
	//ImGui::Text("Sphere 1: center on [0 ,0 , 0] and rad %i", randomRad1);
	//ImGui::Text("Sphere 2: center on [3 ,3 , 3] and rad %i", randomRad2);

	if (sphereCol) {
		ImGui::Text("Intersection succesful!");
	}
	else {
		ImGui::Text("Failed to intersect!");
	}
	(ImGui::Text(" "));
	if (ImGui::MenuItem("Another one!")) {
		int randomRad1 = seed.Int(0, 5);
		Sphere aux1 = Sphere(float3(0, 0, 0), randomRad1);
		int randomRad2 = seed.Int(0, 5);
		Sphere aux2 = Sphere(float3(3, 3, 3), randomRad2);
		sphereCol = aux1.Intersects(aux2);
	}

	ImGui::End();
}
void ShowCylWindow(bool& showCylWindow, LCG seed, bool cylCol) {


	ImGui::Begin("Cylinder checks", &showCylWindow);
	//ImGui::Text("Cylinder 1: center on [0 ,0 , 0] and rad %i", randomRad1);
	//ImGui::Text("Cylinder 2: center on [4 ,0 , 0] and rad %i", randomRad2);

	if (cylCol) {
		ImGui::Text("Intersection succesful!");
	}
	else {
		ImGui::Text("Failed to intersect!");
	}
	(ImGui::Text(" "));
	
	if (ImGui::MenuItem("Another one!")) {
		int randomRad1 = seed.Int(0, 5);
		Cylinder aux1 = Cylinder(float3(0, 0, 0), float3(0, 0, 0), randomRad1);
		int randomRad2 = seed.Int(0, 5);
		Cylinder aux2 = Cylinder(float3(4, 0, 0), float3(4, 0, 0), randomRad2);

		if (randomRad1 + randomRad2 >= 4) //TODO: cheap AF
			cylCol = true;
		else
			cylCol = false;
	}

	ImGui::End();
}

void ShowTriWindow(bool& showTriWindow, LCG seed, bool triCol) {




	ImGui::Begin("Triangle checks");
	//ImGui::Text("Triangle 1: flat side has %i", randomRad1);
	//ImGui::Text("Triangle 2: Is fixed 3 units away", randomRad2);

	if (triCol) {
		ImGui::Text("Intersection succesful!");
	}
	else {
		ImGui::Text("Failed to intersect!");
	}
	(ImGui::Text(" "));
	if (ImGui::MenuItem("Close window")) { showTriWindow = false; }
	if (ImGui::MenuItem("Another one!")) {
		int randomRad1 = seed.Int(0, 5);
		Triangle aux1 = Triangle(float3(0, 0, 0), float3(randomRad1, 0, 0), float3(randomRad1 / 2, 2, 0));
		int randomRad2 = seed.Int(0, 5);
		Triangle aux2 = Triangle(float3(3, 0, 0), float3(3 + randomRad2, 0, 0), float3(3 + randomRad2 / 2, 2, 0));

		if (randomRad1 >= 3) //TODO: cheap AF
			triCol = true;
		else
			triCol = false;
	}

	ImGui::End();
}

void ShowAAABBWindow(bool& showAABBWindow, LCG seed, bool AABBCol) {
	ImGui::Begin("AABB checks");
	//ImGui::Text("AABB 1: contains sphere with center 0,0,0 with rad %i", randomRad1);
	//ImGui::Text("AABB 2: contains sphere with center 4,0,0 with rad %i", randomRad2);

	if (AABBCol) {
		ImGui::Text("Intersection succesful!");
	}
	else {
		ImGui::Text("Failed to intersect!");
	}

	if (ImGui::MenuItem("Close window")) {

		showAABBWindow = false;
	}
	ImGui::End();
}